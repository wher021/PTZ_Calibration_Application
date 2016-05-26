#include "estrap.h"
#include "CalibrationUI.h"

#define NR_ITERATIONS	2
#define PAN_HOME		0
#define TILT_HOME		0

//===========blocking go-to-position
void ptu_go_to(struct cerial *cer, int pan, int tilt){
	uint16_t status;

	if (cpi_ptcmd(cer, &status, OP_PAN_DESIRED_POS_SET, pan) ||
		cpi_ptcmd(cer, &status, OP_TILT_DESIRED_POS_SET, tilt))
	{
		die("Failed to go to min position.\n");
	}
	if (cpi_block_until(cer, NULL, NULL, OP_PAN_CURRENT_POS_GET, pan) ||
		cpi_block_until(cer, NULL, NULL, OP_TILT_CURRENT_POS_GET, tilt))
	{
		die("Blocking for reset completion failed.\n");
	}
}
//=============Reset PTU All
void ptu_reset(struct cerial *cer, cpi_reset_type type){
	uint16_t status;

	if (cpi_ptcmd(cer, &status, OP_RESET, type))
	{
		die("Failed to execute Reset.\n");
	}
	if (cpi_block_until(cer, NULL, NULL, OP_PAN_CURRENT_POS_GET, PAN_HOME) ||
		cpi_block_until(cer, NULL, NULL, OP_TILT_CURRENT_POS_GET, TILT_HOME)){
		die("Blocking failed.\n");
	}
}

class robotControl
{
public:
	robotControl();
	void Connect(std::string address);
	void set_pos(int pan, int tilt);
	int get_pan();
	int get_tilt();
	int get_Pan_Resolution();
	int get_Tilt_Resolution();
	void Disconnect(std::string address);
	void Reset();
	bool get_Robot_Status()
	{
		return robotActive;
	}

private:
	double current_pan, current_tilt, pan_resolution, tilt_resolution;
	struct cerial *cer;
	uint16_t status;
	bool robotActive;
};

robotControl::robotControl()
{
	current_pan = 0;
	current_tilt = 0;
	robotActive = false;
}
int robotControl::get_pan()
{
	cpi_ptcmd(cer, &status, OP_PAN_CURRENT_POS_GET, &current_pan);
	return current_pan;
}
int robotControl::get_tilt()
{
	cpi_ptcmd(cer, &status, OP_TILT_CURRENT_POS_GET, &current_tilt);
	return current_tilt;
}
int robotControl::get_Pan_Resolution()
{
	//OP_PAN_RESOLUTION
	/*
	1 arcsec = 1/3600 of a degree
	so 27 arcsec * 1/3600 arcsec/deg = 0.0075 deg
	//Real deg = 0.0075 * POS
	*/
	cpi_ptcmd(cer, &status, OP_PAN_RESOLUTION, &pan_resolution);
	return pan_resolution;
}
int robotControl::get_Tilt_Resolution()
{
	//OP_PAN_RESOLUTION
	/*
	1 arcsec = 1/3600 of a degree
	so 27 arcsec * 1/3600 = 0.0075 arcsec/ deg
	//Real deg = 0.0075 * POS
	*/

	cpi_ptcmd(cer, &status, OP_TILT_RESOLUTION, &tilt_resolution);
	return tilt_resolution;
}
void robotControl::set_pos(int pan, int tilt)
{
	/*
	OP_PAN_MAX_POSITION
	OP_PAN_MIN_POSITION
	OP_TILT_MAX_POSITION
	OP_TILT_MIN_POSITION
	*/

	ptu_go_to(cer, pan, tilt);
	/*

	if (cpi_ptcmd(cer, &status, OP_PAN_DESIRED_POS_SET, pan) ||
	cpi_ptcmd(cer, &status, OP_TILT_DESIRED_POS_SET, tilt))
	{
	std::cout << "failed to go to pos" << std::endl;
	//die("Failed to go to min position.\n");
	}
	*/

	current_pan = pan;
	current_tilt = tilt;
}

void robotControl::Reset()
{
	/* and go home */
	ptu_go_to(cer, 0, 0);
	std::cout << "Robot Reset" << std::endl;
	current_pan = 0;
	current_tilt = 0;
}

void robotControl::Connect(std::string address)//pass ip adress
{
	std::cout << "trying to connect" << std::endl;
	char *param1 = "CalibrationApp.exe";
	char *param2 = "-p";
	char *undefined_tcp = "tcp:";

	//make a try catch block here because of txtb input
	std::string ip_address = address;

	std::string string_ipaddress = undefined_tcp + ip_address;

	char tmp[39];
	memset(tmp, 0, sizeof tmp);
	strcpy(tmp, string_ipaddress.c_str());

	char *param3;
	param3 = tmp;

	char *command_line[3];

	//char *command_line[3] = { param1, param2, param3 };
	command_line[0] = param1;
	command_line[1] = param2;
	command_line[2] = param3;

	std::cout << "this is whats passed" << std::endl;
	std::cout << command_line[0];
	std::cout << command_line[1];
	std::cout << command_line[2] << std::endl;

	uint16_t status;
	int pn, px, tn, tx, pu, tu;
	int i = 0;
	int argc = 3;

	if ((cer = estrap_in(argc, command_line)) == NULL)
	{
		*command_line = NULL;
		std::cout << "**Connection failed**" << std::endl;
	}
	else//activating robot
	{
		//================================================================//
		// Set terse mode
		if (cpi_ptcmd(cer, &status, OP_FEEDBACK_SET, CPI_ASCII_FEEDBACK_TERSE)){
			die("Failed to set feedback mode.\n");
			printf("Failed to set feedback mode.\n");
		}

		/* read min/max position and speed */
		if (cpi_ptcmd(cer, &status, OP_PAN_MAX_POSITION, &px) ||
			cpi_ptcmd(cer, &status, OP_PAN_MIN_POSITION, &pn) ||
			cpi_ptcmd(cer, &status, OP_TILT_MAX_POSITION, &tx) ||
			cpi_ptcmd(cer, &status, OP_TILT_MIN_POSITION, &tn) ||
			cpi_ptcmd(cer, &status, OP_PAN_UPPER_SPEED_LIMIT_GET, &pu) ||
			cpi_ptcmd(cer, &status, OP_TILT_UPPER_SPEED_LIMIT_GET, &tu)){
			die("Basic unit queries failed.\n");
			printf("Basic unit queries failed.\n");
		}

		/* set desired speed to MAX */
		if (cpi_ptcmd(cer, &status, OP_PAN_DESIRED_SPEED_SET, pu) ||
			cpi_ptcmd(cer, &status, OP_TILT_DESIRED_SPEED_SET, tu)){
			die("Setting PS/TS failed.\n");
			printf("Setting PS/TS failed.\n");
		}

		/* set desired speed to quarter the upper speed */
		if (cpi_ptcmd(cer, &status, OP_PAN_DESIRED_SPEED_SET, pu / 4) ||
			cpi_ptcmd(cer, &status, OP_TILT_DESIRED_SPEED_SET, tu / 4)){
			die("Setting PS/TS failed.\n");
			printf("Setting PS/TS failed.\n");
		}

		/* Test reset */
		//ptu_reset(cer, CPI_RESET_ALL);

		printf("ready to be controlled \n");

		/* and go home */
		ptu_go_to(cer, 0, 0);

		robotActive = true;
	}

}

void robotControl::Disconnect(std::string address)
{

	char *param1 = "CalibrationApp.exe";
	char *param2 = "-p";
	char *undefined_tcp = "tcp:";

	//make a try catch block here because of txtb input
	std::string ip_address = address;

	std::string string_ipaddress = undefined_tcp + ip_address;

	char tmp[39];
	memset(tmp, 0, sizeof tmp);
	strcpy(tmp, string_ipaddress.c_str());

	char *param3;
	param3 = tmp;

	char *command_line[3];

	//char *command_line[3] = { param1, param2, param3 };
	command_line[0] = param1;
	command_line[1] = param2;
	command_line[2] = param3;

	std::cout << "this is whats passed" << std::endl;
	std::cout << command_line[0];
	std::cout << command_line[1];
	std::cout << command_line[2] << std::endl;

	uint16_t status;
	int pn, px, tn, tx, pu, tu;
	int i = 0;
	int argc = 3;

	if ((cer = estrap_in(argc, command_line)) == NULL)
	{
		*command_line = NULL;
		std::cout << "**Connection failed**" << std::endl;
	}
	else//activating robot
	{
		//================================================================//
		// Set terse mode
		if (cpi_ptcmd(cer, &status, OP_FEEDBACK_SET, CPI_ASCII_FEEDBACK_TERSE)){
			die("Failed to set feedback mode.\n");
			printf("Failed to set feedback mode.\n");
		}

		/* read min/max position and speed */
		if (cpi_ptcmd(cer, &status, OP_PAN_MAX_POSITION, &px) ||
			cpi_ptcmd(cer, &status, OP_PAN_MIN_POSITION, &pn) ||
			cpi_ptcmd(cer, &status, OP_TILT_MAX_POSITION, &tx) ||
			cpi_ptcmd(cer, &status, OP_TILT_MIN_POSITION, &tn) ||
			cpi_ptcmd(cer, &status, OP_PAN_UPPER_SPEED_LIMIT_GET, &pu) ||
			cpi_ptcmd(cer, &status, OP_TILT_UPPER_SPEED_LIMIT_GET, &tu)){
			die("Basic unit queries failed.\n");
			printf("Basic unit queries failed.\n");
		}
		/* Test reset */
		ptu_reset(cer, CPI_RESET_ALL);

		/* set desired speed to half the upper speed */
		if (cpi_ptcmd(cer, &status, OP_PAN_DESIRED_SPEED_SET, pu / 2) ||
			cpi_ptcmd(cer, &status, OP_TILT_DESIRED_SPEED_SET, tu / 2)){
			die("Setting PS/TS failed.\n");
			printf("Setting PS/TS failed.\n");
		}

		/* and go home */
		ptu_go_to(cer, 0, 0);

		estrap_out(cer);

		printf("slut \n");
	}
}

