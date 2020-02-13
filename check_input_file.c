#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "symblc_const.h"
#include "check_input_file.h"

#define BUFFER_SIZE 512

//define colors in ansi C
#define ANSI_COLOR_RED       "\x1b[31m"
#define ANSI_COLOR_GREEN     "\x1b[32m"
#define ANSI_COLOR_YELLOW    "\x1b[33m"
#define ANSI_COLOR_RESET     "\x1b[0m"
#define ANSI_COLOR_MAGENTA   "\x1b[35m"

int check_star_rise(DATE , float );

///////////////////////////////////////////////////////////////////////////////////////
// Check the observation schedule table
///////////////////////////////////////////////////////////////////////////////////////
int check_input_file(void)
{
	//----------------------------------------------------
	// Initialize
	char start_string[20],end_string[20],prev_end_string[20];
	char ra_string[15],dec_string[15];
	DATE start,end,now;
	int Nerrors, new_error;
	long prev_end_timestamp,prev_start_timestamp;
	int i,ccd_temp,yes_lines,line =0;
	char filter_string[50],open_flag[2],flat_flag[2];
	char buf[BUFFER_SIZE],error_string[BUFFER_SIZE],warning_string[BUFFER_SIZE];
	char target_name[100];
	int Min_dec=-30,Max_dec=55;
	fpos_t *fposition;
	FILE *fin;
	
	Min_dec = DoGetValue("MIN_DEC");
	Max_dec = DoGetValue("MAX_DEC");
	
	//----------------------------------------------------
	// Read the observation schedule table
	if( ( fin=fopen(TIME_TABLE_FILENAME, "r+") ) == NULL)
	{
		printf("ERROR : Open %s failed\n", TIME_TABLE_FILENAME);
		return 1;
	}
	now= getSystemTime();

	line =0;
	while(!feof(fin) )
	{
		fgets(buf, BUFFER_SIZE, fin);line ++;
		if( !strncmp(buf,"DATA",4) )
					break;
	}
	//After DATA read 3 more lines
	fgets(buf, BUFFER_SIZE, fin);
	fgets(buf, BUFFER_SIZE, fin);
	fgets(buf, BUFFER_SIZE, fin);
	line +=3;
	yes_lines =0;
	prev_start_timestamp =0;
	prev_end_timestamp =0;
	Nerrors=0;
	new_error = 0;
	fposition = (fpos_t *) malloc (sizeof(fpos_t));
	
	//----------------------------------------------------
	// Check observation line by line, and also check their coorelations.
	while(!feof(fin))
	{
		fgetpos(fin, fposition);
		if(!fgets(buf, BUFFER_SIZE, fin)) break;
		line++;
		
		i=sscanf( buf,"%s %s %s %s %s %d %s %s %s",
			open_flag,start_string,end_string,
			ra_string,dec_string,&ccd_temp,
			filter_string,flat_flag,target_name);
	
		if(i<9) continue; //Not all the input variables are read
		
		open_flag[1]='\0';
		flat_flag[1]='\0';
		if(open_flag[0]=='y' || open_flag[0]=='Y')
		{
			yes_lines++;
			sprintf(
				error_string,
				"# Line %d %s ERROR %s :",
				line, ANSI_COLOR_RED,ANSI_COLOR_RESET);
			sprintf(
				warning_string,
				"# Line %d %s WARNING %s :",
				line, ANSI_COLOR_MAGENTA,ANSI_COLOR_RESET);
			start = string2date(start_string);
			end = string2date(end_string);
			//----------------------------------------------------
			//Check the validity of a single observation schedule (the latter) 
			new_error = check_single_line(
				line,
				start,
				end,
				ra_string,
				dec_string,
				Min_dec,
				Max_dec,
				ccd_temp,
				filter_string,
				flat_flag[0]);
			Nerrors += new_error;
			if (new_error){continue;}
			//----------------------------------------------------
			//Check time relative to previous observations schedule (the former) and current one (the latter).
			
			//WARNING if the former stops after the latter starts.
			if(prev_end_timestamp > start.timestamp){
				printf(
					"%s Start time (%s) overlaps previous observation time(%s)!\n",
					warning_string,start_string,prev_end_string);
			}
			
			//WARNING if the former starts after the latter starts.
			if(prev_start_timestamp > start.timestamp){
				printf(
					"%s The start times are not in ascending order (Check %s)\n",
					error_string,start_string);
				Nerrors++;
			}	
			//WARNING if both the former and the latter start within certain minutes.
			if(abs(prev_start_timestamp - start.timestamp)<FIVEMINUTES){
				printf(
					"%s Two Start times are too close (difference must be > 5 mins) (Check %s)\n",
					error_string,start_string);
				Nerrors++;
			}
			//WARNING if the timing for the latter observation is already past, but the targets are not observed yet.
			if(now.timestamp > end.timestamp-HALFHOUR){
				printf(
					"%s End time (%s) already passed!\n",
					warning_string,end_string);
				//Set N
				fsetpos(fin, fposition);
				fseek(fin, 0, SEEK_CUR);
				fputc('N', fin);
				line--;
				continue;
			}		
			//Iterate to the next observation schedule.
			prev_start_timestamp = start.timestamp;
			prev_end_timestamp = end.timestamp;
			strcpy(prev_end_string,end_string);
		}
	}
	fclose(fin);
	free(fposition);
	
	printf("Number of lines with 'Y' flag = %d\n",yes_lines);
	return Nerrors;
}

///////////////////////////////////////////////////////////////////////////////////////
// Function to check only one line
///////////////////////////////////////////////////////////////////////////////////////

int check_single_line(
	int line,
	DATE start,
	DATE end,
	char *ra_string, 
	char *dec_string, 
	int Min_dec,
	int Max_dec,
	int ccd_temp, 
	char *filter_string, 
	char flat_flag){
	//-----------------------------------------------------
	//Initialize
	int i,Nerrors=0,diff_minutes=0,dec_sign=1;
	int ra_h,ra_m,ra_s,dec_d,dec_m,dec_s;
	float RA,DEC;
	char error_string[BUFFER_SIZE],warning_string[BUFFER_SIZE];
	int N_filters,filter_seq[FILTER_TOTAL_NUMBER];
	int filter_exp_time[FILTER_TOTAL_NUMBER],filter_obs_time[FILTER_TOTAL_NUMBER];
	
	sprintf(error_string,"# Line %d %s ERROR %s :",line, ANSI_COLOR_RED,ANSI_COLOR_RESET);
	sprintf(warning_string,"# Line %d %s WARNING %s :",line, ANSI_COLOR_MAGENTA,ANSI_COLOR_RESET);
	
	//-----------------------------------------------------
	//Check the validity of this observation schedule. 
	
	// CHECK LENGTH OF OBSERVATION 
	if(end.timestamp - start.timestamp < FIVEMINUTES){
		printf(
			"%s End - Start time < 5 mins ! (%s - %s)\n",
			error_string,start.string,end.string);
		Nerrors++;
	}	
	if(end.timestamp - start.timestamp > SIXTEENHOURS){
		printf(
			"%s End - Start time > 16 hours !(%s - %s)\n",
			error_string,start.string,end.string);
		Nerrors++;
	}
	// CHECK TEMPERATURE
	if(ccd_temp < -50 || ccd_temp > 20){
		printf(
			"%s Temperature out of bounds!\n",
			error_string);
		Nerrors++;
	}
	//CHECK FLAT
	if(!(flat_flag == 'a' ||  flat_flag == 'b' || flat_flag == 't'|| flat_flag == 'n')){
		printf(
			"%s Unknown flat flag!\n",
			error_string);
		Nerrors++;
	}	
	//-----------------------------------------------------
	//CHECK COORDINATES on mathematic
	//Convert coordinate from string to integers.
	sscanf(ra_string,"%d:%d:%d",&ra_h,&ra_m,&ra_s);
	sscanf(dec_string,"%d:%d:%d",&dec_d,&dec_m,&dec_s);
	if(dec_d<0){dec_d *=-1;dec_sign =-1;}// Sign is negative, integer is positive
	if(dec_m<0){dec_m *=-1;dec_sign =-1;}// Sign is negative, integer is positive
	if(dec_s<0){dec_s *=-1;dec_sign =-1;}// Sign is negative, integer is positive
	// Check whether R.A. is valid on math.
	if((ra_h>=24 ||ra_h<0||ra_m<0||ra_m>=60||ra_s<0||ra_s>=60)){
		printf(
			"%s RA coordinates out of bounds!\n",
			error_string);
		Nerrors++;
	}	
	// Check whether Dec. is valid on math.
	if((dec_d>=60 ||dec_d<=-60||dec_m<=-60||dec_m>60||dec_s<=-60||dec_s>=60)){
		printf(
			"%s DEC coordinates out of bounds!\n",
			error_string);
		Nerrors++;
	}				
	//-----------------------------------------------------
	//CHECK COORDINATES on reality.
	// R.A. unit, Convert hh:mm:ss to degree 
	// Dec. unit, Convert dd:mm:ss to degree	
	RA = (float)ra_h + (float)ra_m/60. + (float)ra_s/3600.;
	DEC= (float)dec_d + (float)dec_m/60. + (float)dec_s/3600.;
	DEC *= (float) dec_sign;
	
	// Check whether Dec. is in the observable range.
	if(DEC<Min_dec || DEC>Max_dec){
		printf(
			"%s DEC out of TAT limits (%d,%d)!\n",
			error_string,Min_dec,Max_dec);
		Nerrors++;
	}
	// Check whether R.A. is in the observable range.
	// Skip RA 00:00:00 Dec 00:00:00 since it is used to taking dark current images.
	if(!(ra_h==0&&ra_m==0&&ra_s==0&&dec_d==0&&dec_m==0&&dec_s==0)){ //is_dark_obs=1;
		// Check the visibility at the begining of the observation.
		diff_minutes = check_star_rise(start, RA);
		if(diff_minutes > 0){
			printf(
				"%s Target is already set at begining (%s) of observation (%d min)!\n",
				error_string,start.string,diff_minutes);
			Nerrors++;
		}
		else if(diff_minutes < 0){
			printf(
				"%s Target will not be risen yet at begining (%s) of observation (%d min)!\n",
				warning_string,start.string,diff_minutes);
		}		
		// Check the visibility at the end of the observation.
		diff_minutes = check_star_rise(end, RA);
		if(diff_minutes >0){
			printf(
				"%s Target will be already set at end (%s) of observation (%d min)!\n",
				error_string,end.string,diff_minutes);
			Nerrors++;
		}
		else if(diff_minutes <0){
			printf(
				"%s Target will not rise at end (%s) of observation! (%d min)\n",
				error_string,end.string,diff_minutes);
			Nerrors++;
		}
	}	
	//replace the ra and dec strings to make use 2 digits 
	sprintf(ra_string,"%02d:%02d:%02d",ra_h,ra_m,ra_s);
	sprintf(dec_string,"%c%02d:%02d:%02d",dec_sign>0?' ':'-',dec_d,dec_m,dec_s);
	
	//CHECK FILTER Sequence
	N_filters = read_filter_string(filter_string,filter_seq,filter_exp_time,filter_obs_time);
	if(N_filters<1) {printf("%s Number of filters %d!\n",error_string,N_filters);Nerrors++;}
	
	for(i=0;i<N_filters;i++){
		if(filter_seq[i]<-1||filter_seq[i]>=FILTER_TOTAL_NUMBER){
			printf(
				"%s %d filter is unknown!\n",
				error_string,i);
			Nerrors++;
		}
		if(filter_exp_time[i]<=0||filter_exp_time[i]>10000){
			printf(
				"%s %d exposure time out of bounds!\n",
				error_string,i);
			Nerrors++;
		}
		if(filter_obs_time[i]<=0||filter_obs_time[i]>10000){
			printf("%s %d observing time out of bounds!\n",
				error_string,i);
			Nerrors++;
		}
	}	
	return Nerrors;
}

///////////////////////////////////////////////////////////////////////////////////////

int insert_obs(
	char *start_string,
	char *end_string, 
	char *ra_string,
	char *dec_string,
	int ccd_temp, 
	char *filter_string, 
	char flat_flag, 
	char *target_name)
{
	int Nerrors =0,line_inserted =0;;
	int Min_dec,Max_dec;
	char input_line[BUFFER_SIZE],buf[BUFFER_SIZE],prev_line[BUFFER_SIZE];
	char temp_filename[BUFFER_SIZE],cmd[BUFFER_SIZE];
	DATE in_start,in_end,line_start;
	FILE *fin,*ftemp;

	
	Min_dec = DoGetValue("MIN_DEC");
	Max_dec = DoGetValue("MAX_DEC");
	in_start = string2date(start_string);
	in_end = string2date(end_string);
	
	
	// First check if any errors
	Nerrors = check_single_line(
			0,
			in_start,
			in_end,
			ra_string, 
			dec_string,
			Min_dec,
			Max_dec,
			ccd_temp,
			filter_string,
			flat_flag);
	
	if(Nerrors) return 0; //errors get out

	//Generate te
	sprintf(input_line,"Y %s %s %s %s %d %s %c %s\n",
			start_string,end_string,ra_string,dec_string,
			ccd_temp,filter_string,flat_flag, target_name);
	
	
	sprintf(temp_filename,"%s.temp",TIME_TABLE_FILENAME);
	
	if( ( fin=fopen(TIME_TABLE_FILENAME, "r") ) == NULL){
		printf("ERROR : Open %s failed\n", TIME_TABLE_FILENAME);
		return 0;
	}
	if( ( ftemp=fopen(temp_filename, "w") ) == NULL){
		fclose(fin);
		printf("ERROR : Could not open temporal file\n");
		return 0;
	}
	while(!feof(fin) ){
		fgets(buf, BUFFER_SIZE, fin);fprintf(ftemp,"%s",buf);
		if( !strncmp(buf,"DATA",4) ) break;
	}
	//After DATA read 3 more lines
	fgets(buf, BUFFER_SIZE, fin);fprintf(ftemp,"%s",buf);
	fgets(buf, BUFFER_SIZE, fin);fprintf(ftemp,"%s",buf);
	fgets(buf, BUFFER_SIZE, fin);fprintf(ftemp,"%s",buf);

	while(!feof(fin))
	{
		if(!fgets(buf, BUFFER_SIZE, fin)) break;
		sscanf(buf,"%*s %s %s",start_string,end_string);
		
		line_start = string2date(start_string);
		if(line_start.timestamp > in_start.timestamp & !line_inserted){
			fprintf(ftemp,"%s",input_line);
			printf("Inserted line: %s",input_line);
			line_inserted =1;
		}
		else if( abs(line_start.timestamp - in_start.timestamp)<FIVEMINUTES){
			printf("%s WARNING %s: Start time too close to another observation (%s,%s)\n",
				ANSI_COLOR_MAGENTA,ANSI_COLOR_RESET,line_start.string, in_start.string);
			printf("Cancel the original observation (set N)\n");
			buf[0] = 'N';
		}
		fprintf(ftemp,"%s",buf);
	}
	// All the dates are earlier than the input line
	if(!line_inserted){
		fprintf(ftemp,"%s",input_line);
		printf("Inserted line: %s",input_line);
	}
	// Close files
	fclose(fin);
	fclose(ftemp);
	//swap both files
	sprintf(cmd,"mv -f %s %s",temp_filename,TIME_TABLE_FILENAME);
	system(cmd);
	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////

int check_star_rise(DATE d, float ra)
{
	// Initialize
	int min;
	DATE ut;
	float LST,hourAngle,diff;
	
	ut= convert2UT(d);
	LST= (float)getLocalSidereal(ut);	
	printf("Local Sidereal Time = %10.6f (hr)\n", LST);
	printf("Right Ascention = %10.6f (hr)\n", ra);
	
	hourAngle = LST - ra;
	// WEST: positive
	// EAST: negative
	// set the range of HA to (-12, 12)
	while (hourAngle < -12.0) hourAngle +=24.0;
	while (hourAngle >  12.0) hourAngle -=24.0;
	//printf("mv: HourAngle= %12.6f\n", hourAngle);
	if(hourAngle > MIN_ALTITUDE){
		//printf("%f", MIN_ALTITUDE);
		diff = hourAngle - MIN_ALTITUDE;
		min =(int) ((diff*60.0)+0.5);
		//status = ALREADY_SET;	
		//printf("mv: HA = %f; Star has already set! (>5.3hr)\n", hourAngle);
	}
	else if(hourAngle < -MIN_ALTITUDE){
		//printf("%f", MIN_ALTITUDE);
		diff = hourAngle + MIN_ALTITUDE;
		min =(int) ((diff*60.0)+0.5);
		//status = NOT_RISE;
		//printf("mv: HA = %f; Star has not risen yet! (<-5.3hr)\n", hourAngle);
	}
	else{
		diff = 0.0;
		min=0;
	}
	return min;
}
