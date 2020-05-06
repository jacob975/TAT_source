#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "symblc_const.h"
#include "weather_func.h"
#include "common_func.h"
//#include "myAstro.h"
#include "fli_func.h"
#include "check_input_file.h"
#include "auto_observe_func.h"

#define TENMINUTES 600
#define ONEHOUR 3600
#define HALFHOUR 1800
#define THREEHOURS 10800
#define FOURHOURS 14400
#define BUFFER_SIZE 512

extern st_tat_info *p_tat_info;

int DoGetObserveTime(float *setpoint, time_t *p_begin_time, time_t *p_end_time)
{
	//----------------------------------------------------------------------
	// Declaration
	FILE *fp;
	char begin_string[15],end_string[15],ra_string[15],dec_string[15];
	char open_flag[2], flat_start[2];
	struct tm ptr_time;
	char *line = NULL;
	size_t len = 0;
	int i,ra_h,ra_m,ra_s,dec_d,dec_m,dec_s;
	char buf[BUFFER_SIZE],temp_string[200];
	int observable = 0, weather_flag = 0;
	int clarity_criterion;
	int before_end_time,end_time_ok;
	char filter_string[100];
	DATE begin_date,end_date,now;
	//----------------------------------------------------------------------
	// Initialize vars
	before_end_time = DoGetValue("WAIT_BEFORE_END_TIME");
	now= getSystemTime();
	//----------------------------------------------------------------------
	// Load observation schedule
	if( ( fp=fopen(TIME_TABLE_FILENAME, "r") ) == NULL)
	{
		sprintf(temp_string,"ERROR: Open %s failed", TIME_TABLE_FILENAME);
		step(temp_string);
		return 0;
	}
	while(!feof(fp) )
	{
		fgets(buf, BUFFER_SIZE, fp);
		if( !strncmp(buf,"DATA",4) )
					break;
	}
	// After DATA read 3 more lines
	fgets(buf, BUFFER_SIZE, fp);
	fgets(buf, BUFFER_SIZE, fp);
	fgets(buf, BUFFER_SIZE, fp);
	
	while(fgets(buf, BUFFER_SIZE, fp))
	{
		//--------------------------------------------------------------------
		// Initialize
		observable = 0;
		// Load input line from observation schedule
		i=sscanf( 
			buf,
			"%s %s %s %s %s %f %s %s %*s",
			open_flag, 
			begin_string,
			end_string,
			ra_string,
			dec_string,
			setpoint,
			filter_string,
			flat_start
		);
		if(i<8) continue; //Quit if some input is missing
		//printf("%s,%s,%s,%d\n",open_flag,begin_string,end_string,*setpoint);
		open_flag[1]='\0';
		flat_start[1]='\0';	
		begin_date = string2date(begin_string);
		end_date = string2date(end_string);
		//-------------------------------------------------------------------
		// Check whether the observation condition is satified or not
		// Weather
		weather_flag = check_previous_weather_conditions_print(1); // be conservative
		if (weather_flag != NORMAL_STATUS) return 0;
		// Start and end time
		observable = check_observable_now(buf);
		if (!observable)
			continue;
		// Show informations of the next target.
		sprintf(
			temp_string,
			"Next target:"
		);
		step(temp_string);
		buf[strlen(buf)-1] = 0;
		step(buf);
		//-------------------------------------------------------------------
		// Copy data from input line to SHM
		// Analising flat time
		strcpy(p_tat_info->obs_info.flat_start , flat_start);
		if (!strcmp(p_tat_info->obs_info.flat_start,"a"))
			step("Lets do flat after observation");
		else if (!strcmp(p_tat_info->obs_info.flat_start,"b"))
			step("Lets do flat before observation");
		else if (!strcmp(p_tat_info->obs_info.flat_start,"t"))
			step("Lets do flat after and before observation");
		else if (!strcmp(p_tat_info->obs_info.flat_start,"n"))
			step("No Flat Fielding in this observation");
		else{
			sprintf(
				temp_string,
				"ERROR: Can not understand this flat \"%s\".",
				p_tat_info->obs_info.flat_start
			);
			step(temp_string);
			break;
		}	
		// Upload RA and DEC to SHM
		sscanf(ra_string,"%d:%d:%d",&ra_h,&ra_m,&ra_s);
		sscanf(dec_string,"%d:%d:%d",&dec_d,&dec_m,&dec_s);
		if(dec_d <0){
			dec_m *=-1;
			dec_s *=-1;
		}
		p_tat_info->obs_info.RA  = (float)ra_h + (float)ra_m/60. + (float)ra_s/3600.;
		p_tat_info->obs_info.DEC = (float)dec_d + (float)dec_m/60. + (float)dec_s/3600.;
		// Get the information of the filters
		p_tat_info->obs_info.N_filters = read_filter_string(
			filter_string, 
			p_tat_info->obs_info.filter_seq, 
			p_tat_info->obs_info.filter_exp_time,
			p_tat_info->obs_info.filter_obs_time
		);
		fclose( fp);
		// Send the observation timing info back to SHM
		ptr_time.tm_year= begin_date.yr - 1900;
		ptr_time.tm_mon= begin_date.mon - 1;
		ptr_time.tm_mday= begin_date.day;
		ptr_time.tm_hour= begin_date.hr;
		ptr_time.tm_min= begin_date.min;
		ptr_time.tm_sec= begin_date.sec;
		ptr_time.tm_isdst= 0;
		*p_begin_time=mktime(&ptr_time);
		
		ptr_time.tm_year= end_date.yr - 1900;
		ptr_time.tm_mon= end_date.mon - 1;
		ptr_time.tm_mday= end_date.day;
		ptr_time.tm_hour= end_date.hr;
		ptr_time.tm_min= end_date.min;
		ptr_time.tm_sec= end_date.sec;
		ptr_time.tm_isdst= 0;
		*p_end_time=mktime(&ptr_time);	
		return 1;
	}
	fclose( fp);
	return 0;
}

int wait4star_rise()
{
	//-----------------------------------------------
	// Declaration
	DATE d;
	float ra_obj;
	int weather_flag;
	int diff_min = 0;
	char systemCmd[BUFSIZ];
	char temp_string[BUFSIZ];
	FILE *fp;
	//-----------------------------------------------
	// Initialization
	// Get the Target's coordinates from the shared memory
	d = getSystemTime();
	ra_obj = (float)p_tat_info->obs_info.RA;
	//-----------------------------------------------
	// Wait until the next target is observable.
	do{
		//-------------------------------------------------------------
		// Check weather
		weather_flag = check_previous_weather_conditions_print(0);//Don't be conservative
		if(weather_flag != NORMAL_STATUS) return weather_flag;	
		//-------------------------------------------------------------
		// Check if emergency status
		if( (fp=fopen(AUTO_CMD_FILENAME,"r")) != NULL)
		{
			fscanf(fp,"%s",temp_string);
			fclose(fp);
			remove( AUTO_CMD_FILENAME);
			if(!strcmp(BREAK_ALL_OBSERVATION ,temp_string))
			{
				step("WARNING: Observation canceled by observer.");
				return EMERGENCY_STATUS;
			}
			else if(!strcmp(DO_NEXT_OBSERVATION ,temp_string))
			{
				step("WARNING: Observer requested to move to next observation.");
				return NEXT_OBSERVATION;
			}
		}
		//-------------------------------------------------------------
		// Check whether the current target is observable or not.
		diff_min = check_star_rise(d, ra_obj);
		// Not rise yet
		if (diff_min < 0){
			step("Wait 1 minute");
			sleep(60);
		}
		// Already set
		else if(diff_min > 0) 
			return NEXT_OBSERVATION;
		// On the sky
		else if(diff_min == 0)
			return NORMAL_STATUS;
		else
			return NEXT_OBSERVATION;
	}while(1);
}

/////////////////////////
//Check if we have multiple observation night
/////////////////////
int check_multiple_observation(char *flat_filter_option, char *dark_exp_option)
{
	int i,N,Nobs=0,begin[7],end[7];
	int filter_present,exp_present;
	DATE begin_date, end_date, now, last_noon, next_noon;
	char buf[BUFFER_SIZE], temp_string[200];
	char open_flag[2], begin_string[20],end_string[20];
	char filter_string[100];
	FILE *fp;
	int observable_now = 0, observable_tonight = 0;	
	int last_noon_status = 1;
	if( ( fp=fopen(TIME_TABLE_FILENAME, "r") ) == NULL)
	{
		sprintf(buf,"ERROR: Open %s failed.",TIME_TABLE_FILENAME);
		step(buf);
		return 0;
	}

	while(!feof(fp))
	{
		fgets(buf, BUFFER_SIZE, fp);
		if( !strncmp(buf,"DATA",4) )
					break;
	}
	//After DATA read 3 more lines
	fgets(buf, BUFFER_SIZE, fp);
	fgets(buf, BUFFER_SIZE, fp);
	fgets(buf, BUFFER_SIZE, fp);
	
	now= getSystemTime();
	//printf("Now = %04d/%02d/%02d %02d:%02d:%02d\n",now.year,now.mon,now.day,now.hr,now.min,now.sec);
	// Initialize the variables
	Nobs=0;
	flat_filter_option[0]='\0';
	dark_exp_option[0]='\0';
	// Get last noon time
	last_noon_status = getLastNoonTime(&last_noon);
	if (last_noon_status){
		sprintf(
			temp_string,
			"Error: No correct last noon time available."
		);
		step(temp_string);
		return Nobs;
	}
	// Get next noon time
	next_noon.mon =last_noon.mon; 
        next_noon.day =last_noon.day+1;
        next_noon.yr  =last_noon.yr; 
        next_noon.hr  =last_noon.hr;
        next_noon.min =last_noon.min;
        next_noon.sec =last_noon.sec; 
        next_noon.year=last_noon.year;
	next_noon.timestamp = get_timestamp(next_noon);
	sprintf(
                next_noon.string,
                "%04d/%02d/%02d %02d:%02d:%02d",
                next_noon.yr,
                next_noon.mon,
                next_noon.day,
                next_noon.hr,
                next_noon.min,
                next_noon.sec
        );
	while(fgets(buf, BUFFER_SIZE, fp)){
		//--------------------------------------------------
		// Initialize
		observable_now = 0;
		observable_tonight = 0;
		//--------------------------------------------------
		// Load input line
		i=sscanf( 
			buf,
			"%s %s %s %*s %*s %*s %s",
			open_flag, 
			begin_string,
			end_string,
			filter_string
		);
		// Quit if some input is missing
		if(i<4) continue; 
		begin_date = string2date(begin_string);
		end_date = string2date(end_string);
		//--------------------------------------------------
		// Check if it is obserable now
		observable_now = check_observable_now(buf);
		// Check if it is observable tonight 	
		if(
			//Nobs &&
			begin_date.timestamp > now.timestamp &&
			begin_date.timestamp < next_noon.timestamp
		) 
			observable_tonight = 1;
		// Add Flat and Dark information from selected lines.
		if (observable_now || observable_tonight){
			begin[Nobs] = begin_date.timestamp;
			end[Nobs] = end_date.timestamp;
			Nobs++;
			update_flat_dark_strings(
				filter_string,
				flat_filter_option,
				dark_exp_option
			);
		}
	}
	fclose(fp);
	return Nobs;
}


///////////////////////////////////////////////////////////////
// Assign the information of the next target into shared memory.
///////////////////////////////////////////////////////////////
int set_current_observation(char *flat_filter_option, char *dark_exp_option, char *schedule_line)
{
	//--------------------------------------------------
	// 0:  Failed
	// 1:  One observable target found
	// -1: Dark mode, taking darks at home.
	// -2: Filled up mode, taking darks between observations.
	//--------------------------------------------------
	// Declaration
	int i,ra_h,ra_m,ra_s;
	int dec_d,dec_m,dec_s;
	DATE begin_date,end_date,now, last_noon, next_noon;
	char buf[BUFFER_SIZE], temp_string[200];
	char begin_string[20],end_string[20],ra_string[15],dec_string[15];
	char open_flag[2],filter_string[100],target_name[100];
	FILE *fp;
	struct tm *ptr_time;
	time_t rawtime;
	fpos_t *fposition;
	int observable = 0, observable_later = 0, observable_future = 0, last_noon_status = 0;
	//--------------------------------------------------
	// Load the observation schedule
	if( ( fp=fopen(TIME_TABLE_FILENAME, "r+") ) == NULL)
	{
		sprintf(buf,"ERROR: Open %s failed.",TIME_TABLE_FILENAME);
		step(buf);
		return 0;
	}

	while(!feof(fp) )
	{
		fgets(buf, BUFFER_SIZE, fp);
		if( !strncmp(buf,"DATA",4) )
					break;
	}
	//After DATA read 3 more lines
	fgets(buf, BUFFER_SIZE, fp);
	fgets(buf, BUFFER_SIZE, fp);
	fgets(buf, BUFFER_SIZE, fp);
	
	now= getSystemTime();

	fposition = (fpos_t *) malloc (sizeof(fpos_t));
	while(!feof(fp))
	{
		//-----------------------------------------------------------------
		// Read the observation line by line
		fgetpos(fp, fposition);
		if(!fgets(buf, BUFFER_SIZE, fp)) break;
		
		i=sscanf(
			buf,
			"%s %s %s %s %s %*s %s %*s %s",
			open_flag,
			begin_string,
			end_string,
			ra_string,
			dec_string,
			filter_string,
			target_name
		);
	
		begin_date = string2date(begin_string);
		end_date = string2date(end_string);	
		if(i<7) continue; // Some input is missing
		// Get last noon time
		last_noon_status = getLastNoonTime(&last_noon);
		if (last_noon_status){
			sprintf(
				temp_string,
				"Error: No correct last noon time available."
			);
			step(temp_string);
			return 0; // No observation tonight
		}
		// Get next noon time
		next_noon.mon =last_noon.mon; 
        	next_noon.day =last_noon.day+1;
        	next_noon.yr  =last_noon.yr; 
        	next_noon.hr  =last_noon.hr;
        	next_noon.min =last_noon.min;
        	next_noon.sec =last_noon.sec; 
        	next_noon.year=last_noon.year;
		next_noon.timestamp = get_timestamp(next_noon);
		sprintf(
        	        next_noon.string,
        	        "%04d/%02d/%02d %02d:%02d:%02d",
        	        next_noon.yr,
        	        next_noon.mon,
        	        next_noon.day,
        	        next_noon.hr,
        	        next_noon.min,
        	        next_noon.sec
        	);
		//-----------------------------------------------------------------
		// Check whether this target is observable or not.
		// The target should be:
		// 1. Not observed before (init 'Y' or 'y')
		// 2. end time not pass yet.
		// 3. In start time 
		observable = check_observable_now(buf);
		observable_later = 
			(open_flag[0] == 'Y' || open_flag[0] == 'y') &&
			begin_date.timestamp >= now.timestamp &&
			begin_date.timestamp <= next_noon.timestamp;
		observable_future = 
			(open_flag[0] == 'Y' || open_flag[0] == 'y') &&
			begin_date.timestamp >= now.timestamp;
		//-----------------------------------------------------------------
		// Skip this target if not observable
		if (!(observable) && !(observable_future)){ 
			// CHANGE THE FLAG
			fsetpos(fp, fposition);
			fseek(fp, 0, SEEK_CUR);
			fputc('N', fp);
			continue;
		}
		//-----------------------------------------------------------------
		// Update target information if observable
		if(observable){
			time (&rawtime);
			ptr_time = localtime(&rawtime);
			
			ptr_time->tm_year= end_date.yr - 1900;
			ptr_time->tm_mon= end_date.mon - 1;
			ptr_time->tm_mday= end_date.day;
			ptr_time->tm_hour= end_date.hr;
			ptr_time->tm_min= end_date.min;
			ptr_time->tm_sec= end_date.sec;
			ptr_time->tm_isdst= 0;
			p_tat_info->obs_info.end_time=mktime(ptr_time);
			
			sscanf(ra_string,"%d:%d:%d",&ra_h,&ra_m,&ra_s);
			sscanf(dec_string,"%d:%d:%d",&dec_d,&dec_m,&dec_s);
			if(dec_d <0){dec_m *= -1; dec_s *= -1;}
			
			// For the shared memory
			p_tat_info->obs_info.RA  = 
				(float)ra_h + 
				(float)ra_m/60.0 + 
				(float)ra_s/3600.0;
			p_tat_info->obs_info.DEC = 
				(float)dec_d + 
				(float)dec_m/60.0 + 
				(float)dec_s/3600.0;
			strcpy(p_tat_info->obs_info.target_name,target_name);
			
			// CHANGE THE FLAG
			//fsetpos(fp, fposition);
			//fseek(fp, 0, SEEK_CUR);
			//fputc('D', fp);
			fclose(fp);
			//free(fposition);
			
			update_flat_dark_strings(
				filter_string,
				flat_filter_option,
				dark_exp_option
			);
			
			if(ra_h ==0 && ra_m ==0 && ra_s ==0 && dec_d ==0 && dec_m ==0 && dec_s ==0){
				strcpy(schedule_line, buf); 
				return -1;
			}
			else{ 
				strcpy(schedule_line, buf); 
				return 1;
			}
		}
		//-------------------------------------------------------------
		// It is not observing time, but there is an observation later
		// Take darks until the begining of next observation
		else if(observable_later){
			time (&rawtime);
			ptr_time = localtime(&rawtime);
			ptr_time->tm_year= begin_date.yr - 1900;
			ptr_time->tm_mon= begin_date.mon - 1;
			ptr_time->tm_mday= begin_date.day;
			ptr_time->tm_hour= begin_date.hr;
			ptr_time->tm_min= begin_date.min;
			ptr_time->tm_sec= begin_date.sec;
			ptr_time->tm_isdst= 0;
			p_tat_info->obs_info.end_time=mktime(ptr_time);
			
			fclose(fp);
			free(fposition);
			return -2; // do flat using dark_exp_option
		}
	}
	// No result
	fclose(fp);
	free(fposition);
	return 0;
}

void update_flat_dark_strings(char *filter_string,char *for_flat,char *for_dark)
{
	int i,j,k,l,exp_time;
	char str[100],filter_char[2],filter_pos[FILTER_TOTAL_NUMBER];
	
	// Get the char of the filters
	get_filter_array_char(filter_pos);
	// Read the filter string
	
	k=0;
	i=0;
	while(k<FILTER_TOTAL_NUMBER)
	{
		j=0;
		while(filter_string[i]!=')')
		{
			if(filter_string[i] == '\0')break;
			str[j] = filter_string[i];
			i++;
			j++;
		}
		if(filter_string[i] == '\0')break; // Get out of main loop
		i++;
		k++;
		str[j]='\0';
		
		for(l=0;l<FILTER_TOTAL_NUMBER;l++)
		{
			if(str[0] == filter_pos[l])
			{
				// Add filter to string
				filter_char[0] = str[0];
				filter_char[1] = '\0';
				add_option_tostring(for_flat,filter_char,0);
				
				// Now add the eposure time for dark
				str[0] = ' ';
				str[1] = ' ';
				exp_time = atoi(str);
				if(exp_time >0 && exp_time < 3000)
				{
					sprintf(str,"%d", exp_time);
					add_option_tostring(for_dark,str,1);
				}
			}
		}
	}
}




void add_option_tostring(char *string, char *option, int fordark)
{
	char string_compare[100], option_compare[30];
	
	if(fordark)
	{
		sprintf(string_compare,",%s,",string);
		sprintf(option_compare,",%s,",option);
	}
	else
	{
		strcpy(string_compare,string);
		strcpy(option_compare,option);
	}
	
	if(strstr(string_compare,option_compare) == NULL)
	{
		if(string[0] == '\0') strcpy(string,option);
		else 
		{
			if(fordark)strcat(string,",");
			
			strcat(string,option);
		}
	}
}

