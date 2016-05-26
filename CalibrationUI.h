#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include "ALGLIB\interpolation.h"//Interpolation library

using namespace System;
using namespace System::IO::Ports;
using namespace System::Threading;
using namespace System::Windows::Forms;
using namespace System::Collections::Generic;

#include "zoom_cam_API.h"

#include <opencv2/video/tracking.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>
#include <opencv\highgui.h>
#include "opencv2/imgproc/imgproc.hpp"

#include <msclr\marshal_cppstd.h>//converts System string to std strings

extern "C" {
	//#include "C:\Users\u030373\Desktop\DekstopApp\MyApp\CalibrationApp\CalibrationApp\robot_API.h"
#include "robot_API.h"
}

#include "myVideoStreamerClass.h"
#include "PortChatClass.h"
#include "Add_Position_Dialog_Box.h"
#include "Signal_lamp_data.h"
#include "Devices.h"

//C++ global variables
SignalLamp_Container **signal_lamp_data = new SignalLamp_Container*[];//dynamic array
SignalLamp_Container signal_lamp_data_static[100][100];
bool Signal_Data_Exist = false;
int free_nr_rows = 0;

//PTZ control variables
robotControl *myRobot = new robotControl();
std::string Str_ch0, Str_ch1;
int Int_ch0, Int_ch1;
double resolution_pan;
double resolution_tilt;

int pan = 0;
int tilt = 0;
unsigned char PTZ_stepSize = 10;

//Zoom cam variables
Client *ZoomClient = new Client();
char optical_zoom_level[18];

char buffer_AutoFocusOn[16] = { 0x00, 0x00, 0x00, 0x0C, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x46, 0x30, 0x45, 0x30, 0x30 };
char buffer_AutoFocusOff[16] = { 0x00, 0x00, 0x00, 0x0C, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x46, 0x30, 0x45, 0x30, 0x38 };

char buffer_FocusFar[16] = { 0x00, 0x00, 0x00, 0x0C, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x43, 0x42, 0x42, 0x41, 0x39 };//find
char buffer_FocusNear[16] = { 0x00, 0x00, 0x00, 0x0C, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x43, 0x42, 0x42, 0x41, 0x41 };//find
char buffer_FocusStop[16] = { 0x00, 0x00, 0x00, 0x0C, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x43, 0x42, 0x42, 0x46, 0x45 };//find

char buffer_zoomin[16] = { 0x00, 0x00, 0x00, 0x0C, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x43, 0x42, 0x42, 0x39, 0x39 };
char buffer_zoomout[16] = { 0x00, 0x00, 0x00, 0x0C, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x43, 0x42, 0x42, 0x39, 0x42 };
char buffer_stopzoom[16] = { 0x00, 0x00, 0x00, 0x0C, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x43, 0x42, 0x42, 0x46, 0x45 };

namespace CalibrationApp {
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace cv;
	using namespace std;

	public ref class CalibrationUI : public System::Windows::Forms::Form
	{
		delegate void Update_TextBox_Label_delegate(System::String^ ch0Txt, System::String^ ch1Txt, int, int);
		delegate void Update_Listbox_delegate(System::String^ device, double pan, double tilt);

	public:

		CalibrationUI(void)
		{
			InitializeComponent();
			btnRobotTst->TabStop = false;
			Btn_Connect_Ptz->TabStop = false;
			btn_ZoomCam_Switch->TabStop = false;
			upBtn->TabStop = false;
			downBtn->TabStop = false;
			leftbtn->TabStop = false;
			rightbtn->TabStop = false;
			rstButton->TabStop = false;
			txt_PTZ_ip->TabStop = false;
			panTxt->TabStop = false;
			tiltTxt->TabStop = false;

			this->KeyPreview = true;//make form listen to keypresses!

			populateComboBoxSerialPorts();//retrieve System Serial ports
			populateZoomBox();

			backgroundWorker1->RunWorkerAsync();
			/*
			btn_digital_zoomIn->Enabled = false;
			btn_digital_zoomOut->Enabled = false;
			rB_IR_DZ->Enabled = false;
			rB_ZoomCam_DZ->Enabled = false;
			*/
		}

		void UpdateGUI(System::String^ ch0Txt, System::String^ ch1Txt, int pan, int tilt) {

			if (this->lbCh0->InvokeRequired || this->lbCh1->InvokeRequired || this->panTxt->InvokeRequired || this->tiltTxt->InvokeRequired
				|| this->txtPanDeg->InvokeRequired || this->txtTiltDeg->InvokeRequired)
			{
				Update_TextBox_Label_delegate^ d = gcnew Update_TextBox_Label_delegate(this, &CalibrationUI::UpdateGUI);
				this->Invoke(d, gcnew array<Object^> { ch0Txt, ch1Txt, pan, tilt });
			}
			else {
				this->lbCh0->Text = ch0Txt;
				this->lbCh1->Text = ch1Txt;
				this->panTxt->Text = pan.ToString();
				this->tiltTxt->Text = tilt.ToString();
				this->txtPanDeg->Text = (pan * resolution_pan).ToString();
				this->txtTiltDeg->Text = (tilt * resolution_tilt).ToString();
			}
		}

		void UpdateGUI2(System::String^ device, double pan, double tilt) {

			if (this->lstCalibrationPreview->InvokeRequired || this->lbl_offset_pan->InvokeRequired || this->lbl_offset_tilt->InvokeRequired)
			{
				Update_Listbox_delegate^ d = gcnew Update_Listbox_delegate(this, &CalibrationUI::UpdateGUI2);
				this->Invoke(d, gcnew array<Object^> { device, pan, tilt });
			}
			else 
			{
				System::String ^item = device->PadRight(20 - device->Length) + "\t" + "p= " + pan.ToString()->PadRight(5 - pan.ToString()->Length) + "\t" + "t= " + tilt.ToString();
				this->lstCalibrationPreview->Items->Add(item);
				double pan_offset = (pan - ref_device_data->pan_deg);
				double tilt_offset = (tilt - ref_device_data->tilt_deg);
				System::String ^p_offset = pan_offset.ToString();
				System::String ^t_offset = tilt_offset.ToString();
				p_offset = p_offset->PadRight(5);
				t_offset = t_offset->PadRight(5);
				p_offset = p_offset->Substring(0, 5);
				t_offset = t_offset->Substring(0, 5);//extend with zero first!

				this->lbl_offset_pan->Text = "p= " + p_offset + "°";
				this->lbl_offset_tilt->Text = "t= " + t_offset + "°";
			}
		}

	protected:

		~CalibrationUI()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  btn_ZoomCam_Switch;



	private: System::Windows::Forms::Button^  btnRobotTst;
	private: System::Windows::Forms::TextBox^  txt_PTZ_ip;


	private: System::Windows::Forms::Button^  rstButton;
	private: System::Windows::Forms::Button^  leftbtn;
	private: System::Windows::Forms::Button^  Btn_Connect_Ptz;



	private: System::Windows::Forms::Button^  rightbtn;
	private: System::Windows::Forms::Button^  upBtn;
	private: System::Windows::Forms::Button^  downBtn;
	private: System::Windows::Forms::TextBox^  panTxt;
	private: System::Windows::Forms::TextBox^  tiltTxt;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::TextBox^  txt_Zoom_ip;
	private: System::Windows::Forms::Button^  Btn_Connect_Zoom;
	private: System::Windows::Forms::Button^  btn_auto_focus;

	private: System::Windows::Forms::TextBox^  txtPanDeg;
	private: System::Windows::Forms::TextBox^  txtTiltDeg;
	private: System::Windows::Forms::Button^  btn_IRCam_Switch;

	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label4;

	private: System::ComponentModel::IContainer^  components;
	private: System::Windows::Forms::Button^  btn_digital_zoomIn;
	private: System::Windows::Forms::Button^  btn_manual_focus;



	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::TextBox^  txt_IR_ip;
	private: System::Windows::Forms::Button^  Btn_Connect_IR;

	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::TextBox^  txt_Zoom_Control_Address;
	private: System::Windows::Forms::Button^  btn_connect_control_zoom;

	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::ComboBox^  cbPorts;

	private: System::Windows::Forms::Button^  btnSerialConnect;

	private: System::Windows::Forms::Button^  btn_start_Signal_Lamp_Calibration;
	private: System::Windows::Forms::ListBox^  lstCalibrationPreview;
	private: System::Windows::Forms::Button^  btn_save_pos;
	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::Label^  label9;
	private: System::Windows::Forms::Button^  btn_Delete_Position;
	private: System::Windows::Forms::Label^  label10;
	private: System::Windows::Forms::TrackBar^  Bar_Resolution;


	private: System::Windows::Forms::TextBox^  txt_Ptz_Resolution;
	private: System::Windows::Forms::Button^  btn_digital_zoomOut;
	private: System::Windows::Forms::RadioButton^  rB_ZoomCam_DZ;


	private: System::Windows::Forms::RadioButton^  rB_IR_DZ;

	private: System::Windows::Forms::ListBox^  lst_Zoom_box;
	private: System::Windows::Forms::Button^  btn_focus_far;
	private: System::Windows::Forms::Button^  btn_focus_near;
	private: System::Windows::Forms::Label^  label12;
	private: System::Windows::Forms::Label^  label13;
	private: System::Windows::Forms::Label^  lbCh1;
	private: System::Windows::Forms::Label^  lbCh0;
	private: System::ComponentModel::BackgroundWorker^  backgroundWorker1;
	private: System::ComponentModel::BackgroundWorker^  robotCalibrationThread;

			 VideoStreamer^ myVideoStreamerObject = gcnew VideoStreamer();
			 Thread^ ZoomCamThread = gcnew Thread(gcnew ParameterizedThreadStart(myVideoStreamerObject, &VideoStreamer::StreamZoomVideo));

			 Thread^ IRCamThread = gcnew Thread(gcnew ParameterizedThreadStart(myVideoStreamerObject, &VideoStreamer::StreamIRVideo));

			 PortChat^ mySerial = gcnew PortChat();
			 Thread^ readThread = gcnew Thread(gcnew ThreadStart(PortChat::Read));

			 Device_Container ^ref_device_data = gcnew Device_Container();
			 Device_Container ^device_data = gcnew Device_Container();

			 SerialPort ^_serialPort = gcnew SerialPort();
			 // Thread^ serialthread = gcnew Thread(gcnew ThreadStart(&SerialPort::tst));//the class needs to be managed

	private: System::Windows::Forms::TextBox^  txt_Setpos_Pan;
	private: System::Windows::Forms::TextBox^  txt_Setpos_Tilt;


	private: System::Windows::Forms::Label^  label14;
	private: System::Windows::Forms::Label^  label15;
	private: System::Windows::Forms::Label^  label16;
	private: System::Windows::Forms::Button^  btn_setPos;
	private: System::Windows::Forms::TextBox^  txt_Pan_step_size;
	private: System::Windows::Forms::TextBox^  txt_Tilt_step_size;


	private: System::Windows::Forms::Label^  label17;
	private: System::Windows::Forms::Label^  label18;
	private: System::Windows::Forms::TextBox^  txt_Row_size;
	private: System::Windows::Forms::TextBox^  txt_Col_size;


	private: System::Windows::Forms::Label^  label19;
	private: System::Windows::Forms::Label^  label20;
	private: System::Windows::Forms::Label^  lbl_pan_arcsec;
	private: System::Windows::Forms::Label^  lbl_tilt_arcsec;
	private: System::Windows::Forms::Label^  label22;
	private: System::Windows::Forms::Label^  lbl_offset_tilt;
	private: System::Windows::Forms::Label^  lbl_offset_pan;
	private: System::Windows::Forms::Button^  btn_Grid;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::Label^  label23;
	private: System::Windows::Forms::Button^  btn_goto_grid;

	private: System::Windows::Forms::Label^  label24;
	private: System::Windows::Forms::TextBox^  txt_row_nr;

	private: System::Windows::Forms::TextBox^  txt_col_nr;
private: System::Windows::Forms::GroupBox^  groupBox2;
private: System::Windows::Forms::GroupBox^  groupBox3;
private: System::Windows::Forms::Label^  label11;






	private: System::Windows::Forms::Label^  label21;




#pragma region Windows Form Designer generated code

			 void InitializeComponent(void)
			 {
				 this->btn_ZoomCam_Switch = (gcnew System::Windows::Forms::Button());
				 this->btnRobotTst = (gcnew System::Windows::Forms::Button());
				 this->txt_PTZ_ip = (gcnew System::Windows::Forms::TextBox());
				 this->rstButton = (gcnew System::Windows::Forms::Button());
				 this->leftbtn = (gcnew System::Windows::Forms::Button());
				 this->Btn_Connect_Ptz = (gcnew System::Windows::Forms::Button());
				 this->rightbtn = (gcnew System::Windows::Forms::Button());
				 this->upBtn = (gcnew System::Windows::Forms::Button());
				 this->downBtn = (gcnew System::Windows::Forms::Button());
				 this->panTxt = (gcnew System::Windows::Forms::TextBox());
				 this->tiltTxt = (gcnew System::Windows::Forms::TextBox());
				 this->label1 = (gcnew System::Windows::Forms::Label());
				 this->label2 = (gcnew System::Windows::Forms::Label());
				 this->txt_Zoom_ip = (gcnew System::Windows::Forms::TextBox());
				 this->Btn_Connect_Zoom = (gcnew System::Windows::Forms::Button());
				 this->btn_auto_focus = (gcnew System::Windows::Forms::Button());
				 this->txtPanDeg = (gcnew System::Windows::Forms::TextBox());
				 this->txtTiltDeg = (gcnew System::Windows::Forms::TextBox());
				 this->btn_IRCam_Switch = (gcnew System::Windows::Forms::Button());
				 this->label3 = (gcnew System::Windows::Forms::Label());
				 this->label4 = (gcnew System::Windows::Forms::Label());
				 this->btn_digital_zoomIn = (gcnew System::Windows::Forms::Button());
				 this->btn_manual_focus = (gcnew System::Windows::Forms::Button());
				 this->label5 = (gcnew System::Windows::Forms::Label());
				 this->txt_IR_ip = (gcnew System::Windows::Forms::TextBox());
				 this->Btn_Connect_IR = (gcnew System::Windows::Forms::Button());
				 this->label6 = (gcnew System::Windows::Forms::Label());
				 this->txt_Zoom_Control_Address = (gcnew System::Windows::Forms::TextBox());
				 this->btn_connect_control_zoom = (gcnew System::Windows::Forms::Button());
				 this->label7 = (gcnew System::Windows::Forms::Label());
				 this->cbPorts = (gcnew System::Windows::Forms::ComboBox());
				 this->btnSerialConnect = (gcnew System::Windows::Forms::Button());
				 this->btn_start_Signal_Lamp_Calibration = (gcnew System::Windows::Forms::Button());
				 this->lstCalibrationPreview = (gcnew System::Windows::Forms::ListBox());
				 this->btn_save_pos = (gcnew System::Windows::Forms::Button());
				 this->label8 = (gcnew System::Windows::Forms::Label());
				 this->label9 = (gcnew System::Windows::Forms::Label());
				 this->btn_Delete_Position = (gcnew System::Windows::Forms::Button());
				 this->label10 = (gcnew System::Windows::Forms::Label());
				 this->Bar_Resolution = (gcnew System::Windows::Forms::TrackBar());
				 this->txt_Ptz_Resolution = (gcnew System::Windows::Forms::TextBox());
				 this->btn_digital_zoomOut = (gcnew System::Windows::Forms::Button());
				 this->rB_ZoomCam_DZ = (gcnew System::Windows::Forms::RadioButton());
				 this->rB_IR_DZ = (gcnew System::Windows::Forms::RadioButton());
				 this->lst_Zoom_box = (gcnew System::Windows::Forms::ListBox());
				 this->btn_focus_far = (gcnew System::Windows::Forms::Button());
				 this->btn_focus_near = (gcnew System::Windows::Forms::Button());
				 this->label12 = (gcnew System::Windows::Forms::Label());
				 this->label13 = (gcnew System::Windows::Forms::Label());
				 this->lbCh1 = (gcnew System::Windows::Forms::Label());
				 this->lbCh0 = (gcnew System::Windows::Forms::Label());
				 this->backgroundWorker1 = (gcnew System::ComponentModel::BackgroundWorker());
				 this->robotCalibrationThread = (gcnew System::ComponentModel::BackgroundWorker());
				 this->txt_Setpos_Pan = (gcnew System::Windows::Forms::TextBox());
				 this->txt_Setpos_Tilt = (gcnew System::Windows::Forms::TextBox());
				 this->label14 = (gcnew System::Windows::Forms::Label());
				 this->label15 = (gcnew System::Windows::Forms::Label());
				 this->label16 = (gcnew System::Windows::Forms::Label());
				 this->btn_setPos = (gcnew System::Windows::Forms::Button());
				 this->txt_Pan_step_size = (gcnew System::Windows::Forms::TextBox());
				 this->txt_Tilt_step_size = (gcnew System::Windows::Forms::TextBox());
				 this->label17 = (gcnew System::Windows::Forms::Label());
				 this->label18 = (gcnew System::Windows::Forms::Label());
				 this->txt_Row_size = (gcnew System::Windows::Forms::TextBox());
				 this->txt_Col_size = (gcnew System::Windows::Forms::TextBox());
				 this->label19 = (gcnew System::Windows::Forms::Label());
				 this->label20 = (gcnew System::Windows::Forms::Label());
				 this->lbl_pan_arcsec = (gcnew System::Windows::Forms::Label());
				 this->lbl_tilt_arcsec = (gcnew System::Windows::Forms::Label());
				 this->label21 = (gcnew System::Windows::Forms::Label());
				 this->label22 = (gcnew System::Windows::Forms::Label());
				 this->lbl_offset_tilt = (gcnew System::Windows::Forms::Label());
				 this->lbl_offset_pan = (gcnew System::Windows::Forms::Label());
				 this->btn_Grid = (gcnew System::Windows::Forms::Button());
				 this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
				 this->btn_goto_grid = (gcnew System::Windows::Forms::Button());
				 this->label24 = (gcnew System::Windows::Forms::Label());
				 this->txt_row_nr = (gcnew System::Windows::Forms::TextBox());
				 this->txt_col_nr = (gcnew System::Windows::Forms::TextBox());
				 this->label23 = (gcnew System::Windows::Forms::Label());
				 this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
				 this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
				 this->label11 = (gcnew System::Windows::Forms::Label());
				 (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->Bar_Resolution))->BeginInit();
				 this->groupBox1->SuspendLayout();
				 this->groupBox2->SuspendLayout();
				 this->groupBox3->SuspendLayout();
				 this->SuspendLayout();
				 // 
				 // btn_ZoomCam_Switch
				 // 
				 this->btn_ZoomCam_Switch->Location = System::Drawing::Point(209, 93);
				 this->btn_ZoomCam_Switch->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
				 this->btn_ZoomCam_Switch->Name = L"btn_ZoomCam_Switch";
				 this->btn_ZoomCam_Switch->Size = System::Drawing::Size(100, 47);
				 this->btn_ZoomCam_Switch->TabIndex = 0;
				 this->btn_ZoomCam_Switch->Text = L"ZoomCam on/off";
				 this->btn_ZoomCam_Switch->UseVisualStyleBackColor = true;
				 this->btn_ZoomCam_Switch->Click += gcnew System::EventHandler(this, &CalibrationUI::Zoom_Cam_Click);
				 // 
				 // btnRobotTst
				 // 
				 this->btnRobotTst->Location = System::Drawing::Point(47, 93);
				 this->btnRobotTst->Margin = System::Windows::Forms::Padding(4);
				 this->btnRobotTst->Name = L"btnRobotTst";
				 this->btnRobotTst->Size = System::Drawing::Size(100, 47);
				 this->btnRobotTst->TabIndex = 2;
				 this->btnRobotTst->Text = L"RobotTest";
				 this->btnRobotTst->UseVisualStyleBackColor = true;
				 this->btnRobotTst->Click += gcnew System::EventHandler(this, &CalibrationUI::btnRobotTst_Click);
				 // 
				 // txt_PTZ_ip
				 // 
				 this->txt_PTZ_ip->Location = System::Drawing::Point(47, 32);
				 this->txt_PTZ_ip->Name = L"txt_PTZ_ip";
				 this->txt_PTZ_ip->Size = System::Drawing::Size(100, 22);
				 this->txt_PTZ_ip->TabIndex = 3;
				 // 
				 // rstButton
				 // 
				 this->rstButton->Location = System::Drawing::Point(134, 309);
				 this->rstButton->Name = L"rstButton";
				 this->rstButton->Size = System::Drawing::Size(75, 23);
				 this->rstButton->TabIndex = 4;
				 this->rstButton->Text = L"rstButton";
				 this->rstButton->UseVisualStyleBackColor = true;
				 this->rstButton->Click += gcnew System::EventHandler(this, &CalibrationUI::Button_Click);
				 // 
				 // leftbtn
				 // 
				 this->leftbtn->Location = System::Drawing::Point(53, 309);
				 this->leftbtn->Name = L"leftbtn";
				 this->leftbtn->Size = System::Drawing::Size(75, 23);
				 this->leftbtn->TabIndex = 5;
				 this->leftbtn->Text = L"<";
				 this->leftbtn->UseVisualStyleBackColor = true;
				 this->leftbtn->Click += gcnew System::EventHandler(this, &CalibrationUI::Button_Click);
				 // 
				 // Btn_Connect_Ptz
				 // 
				 this->Btn_Connect_Ptz->Location = System::Drawing::Point(47, 60);
				 this->Btn_Connect_Ptz->Name = L"Btn_Connect_Ptz";
				 this->Btn_Connect_Ptz->Size = System::Drawing::Size(100, 28);
				 this->Btn_Connect_Ptz->TabIndex = 6;
				 this->Btn_Connect_Ptz->TabStop = false;
				 this->Btn_Connect_Ptz->Text = L"Connect";
				 this->Btn_Connect_Ptz->UseVisualStyleBackColor = true;
				 this->Btn_Connect_Ptz->Click += gcnew System::EventHandler(this, &CalibrationUI::PTZ_connectBtn_Click);
				 // 
				 // rightbtn
				 // 
				 this->rightbtn->Location = System::Drawing::Point(215, 309);
				 this->rightbtn->Name = L"rightbtn";
				 this->rightbtn->Size = System::Drawing::Size(75, 23);
				 this->rightbtn->TabIndex = 7;
				 this->rightbtn->Text = L">";
				 this->rightbtn->UseVisualStyleBackColor = true;
				 this->rightbtn->Click += gcnew System::EventHandler(this, &CalibrationUI::Button_Click);
				 // 
				 // upBtn
				 // 
				 this->upBtn->Location = System::Drawing::Point(134, 280);
				 this->upBtn->Name = L"upBtn";
				 this->upBtn->Size = System::Drawing::Size(75, 23);
				 this->upBtn->TabIndex = 8;
				 this->upBtn->Text = L"^";
				 this->upBtn->UseVisualStyleBackColor = true;
				 this->upBtn->Click += gcnew System::EventHandler(this, &CalibrationUI::Button_Click);
				 // 
				 // downBtn
				 // 
				 this->downBtn->Location = System::Drawing::Point(134, 338);
				 this->downBtn->Name = L"downBtn";
				 this->downBtn->Size = System::Drawing::Size(75, 23);
				 this->downBtn->TabIndex = 9;
				 this->downBtn->Text = L"v";
				 this->downBtn->UseVisualStyleBackColor = true;
				 this->downBtn->Click += gcnew System::EventHandler(this, &CalibrationUI::Button_Click);
				 // 
				 // panTxt
				 // 
				 this->panTxt->Enabled = false;
				 this->panTxt->Location = System::Drawing::Point(46, 192);
				 this->panTxt->Name = L"panTxt";
				 this->panTxt->ReadOnly = true;
				 this->panTxt->Size = System::Drawing::Size(100, 22);
				 this->panTxt->TabIndex = 10;
				 this->panTxt->TabStop = false;
				 // 
				 // tiltTxt
				 // 
				 this->tiltTxt->Enabled = false;
				 this->tiltTxt->Location = System::Drawing::Point(46, 228);
				 this->tiltTxt->Name = L"tiltTxt";
				 this->tiltTxt->ReadOnly = true;
				 this->tiltTxt->Size = System::Drawing::Size(100, 22);
				 this->tiltTxt->TabIndex = 11;
				 this->tiltTxt->TabStop = false;
				 // 
				 // label1
				 // 
				 this->label1->AutoSize = true;
				 this->label1->Location = System::Drawing::Point(44, 9);
				 this->label1->Name = L"label1";
				 this->label1->Size = System::Drawing::Size(91, 17);
				 this->label1->TabIndex = 12;
				 this->label1->Text = L"PTZ Address";
				 // 
				 // label2
				 // 
				 this->label2->AutoSize = true;
				 this->label2->Location = System::Drawing::Point(206, 9);
				 this->label2->Name = L"label2";
				 this->label2->Size = System::Drawing::Size(171, 17);
				 this->label2->TabIndex = 13;
				 this->label2->Text = L"Zoom Cam Address(Multi)";
				 // 
				 // txt_Zoom_ip
				 // 
				 this->txt_Zoom_ip->Location = System::Drawing::Point(209, 31);
				 this->txt_Zoom_ip->Name = L"txt_Zoom_ip";
				 this->txt_Zoom_ip->Size = System::Drawing::Size(100, 22);
				 this->txt_Zoom_ip->TabIndex = 14;
				 // 
				 // Btn_Connect_Zoom
				 // 
				 this->Btn_Connect_Zoom->Location = System::Drawing::Point(209, 60);
				 this->Btn_Connect_Zoom->Name = L"Btn_Connect_Zoom";
				 this->Btn_Connect_Zoom->Size = System::Drawing::Size(100, 28);
				 this->Btn_Connect_Zoom->TabIndex = 15;
				 this->Btn_Connect_Zoom->TabStop = false;
				 this->Btn_Connect_Zoom->Text = L"Connect Zoom";
				 this->Btn_Connect_Zoom->UseVisualStyleBackColor = true;
				 this->Btn_Connect_Zoom->Click += gcnew System::EventHandler(this, &CalibrationUI::Zoom_Connect_Click);
				 // 
				 // btn_auto_focus
				 // 
				 this->btn_auto_focus->Location = System::Drawing::Point(394, 95);
				 this->btn_auto_focus->Margin = System::Windows::Forms::Padding(4);
				 this->btn_auto_focus->Name = L"btn_auto_focus";
				 this->btn_auto_focus->Size = System::Drawing::Size(100, 28);
				 this->btn_auto_focus->TabIndex = 16;
				 this->btn_auto_focus->Text = L"AutoFocus";
				 this->btn_auto_focus->UseVisualStyleBackColor = true;
				 this->btn_auto_focus->Click += gcnew System::EventHandler(this, &CalibrationUI::ZoomCam_Focus_Button_Click);
				 // 
				 // txtPanDeg
				 // 
				 this->txtPanDeg->Enabled = false;
				 this->txtPanDeg->Location = System::Drawing::Point(204, 192);
				 this->txtPanDeg->Name = L"txtPanDeg";
				 this->txtPanDeg->ReadOnly = true;
				 this->txtPanDeg->Size = System::Drawing::Size(100, 22);
				 this->txtPanDeg->TabIndex = 17;
				 this->txtPanDeg->TabStop = false;
				 // 
				 // txtTiltDeg
				 // 
				 this->txtTiltDeg->Enabled = false;
				 this->txtTiltDeg->Location = System::Drawing::Point(204, 228);
				 this->txtTiltDeg->Name = L"txtTiltDeg";
				 this->txtTiltDeg->ReadOnly = true;
				 this->txtTiltDeg->Size = System::Drawing::Size(100, 22);
				 this->txtTiltDeg->TabIndex = 18;
				 this->txtTiltDeg->TabStop = false;
				 // 
				 // btn_IRCam_Switch
				 // 
				 this->btn_IRCam_Switch->Location = System::Drawing::Point(622, 93);
				 this->btn_IRCam_Switch->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
				 this->btn_IRCam_Switch->Name = L"btn_IRCam_Switch";
				 this->btn_IRCam_Switch->Size = System::Drawing::Size(100, 47);
				 this->btn_IRCam_Switch->TabIndex = 19;
				 this->btn_IRCam_Switch->Text = L"IR Cam on/off";
				 this->btn_IRCam_Switch->UseVisualStyleBackColor = true;
				 this->btn_IRCam_Switch->Click += gcnew System::EventHandler(this, &CalibrationUI::IR_Cam_Click);
				 // 
				 // label3
				 // 
				 this->label3->AutoSize = true;
				 this->label3->Location = System::Drawing::Point(201, 172);
				 this->label3->Name = L"label3";
				 this->label3->Size = System::Drawing::Size(126, 17);
				 this->label3->TabIndex = 20;
				 this->label3->Text = L"Ptz positions (deg)";
				 // 
				 // label4
				 // 
				 this->label4->AutoSize = true;
				 this->label4->Location = System::Drawing::Point(43, 172);
				 this->label4->Name = L"label4";
				 this->label4->Size = System::Drawing::Size(148, 17);
				 this->label4->TabIndex = 21;
				 this->label4->Text = L"Ptz positions (relative)";
				 // 
				 // btn_digital_zoomIn
				 // 
				 this->btn_digital_zoomIn->Location = System::Drawing::Point(394, 387);
				 this->btn_digital_zoomIn->Name = L"btn_digital_zoomIn";
				 this->btn_digital_zoomIn->Size = System::Drawing::Size(146, 28);
				 this->btn_digital_zoomIn->TabIndex = 22;
				 this->btn_digital_zoomIn->Text = L"Digital_Zoom +";
				 this->btn_digital_zoomIn->UseVisualStyleBackColor = true;
				 this->btn_digital_zoomIn->Click += gcnew System::EventHandler(this, &CalibrationUI::btn_Digital_Zoom_Click);
				 // 
				 // btn_manual_focus
				 // 
				 this->btn_manual_focus->Location = System::Drawing::Point(395, 130);
				 this->btn_manual_focus->Name = L"btn_manual_focus";
				 this->btn_manual_focus->Size = System::Drawing::Size(100, 28);
				 this->btn_manual_focus->TabIndex = 23;
				 this->btn_manual_focus->Text = L"ManualFocus";
				 this->btn_manual_focus->UseVisualStyleBackColor = true;
				 this->btn_manual_focus->Click += gcnew System::EventHandler(this, &CalibrationUI::ZoomCam_Focus_Button_Click);
				 // 
				 // label5
				 // 
				 this->label5->AutoSize = true;
				 this->label5->Location = System::Drawing::Point(619, 9);
				 this->label5->Name = L"label5";
				 this->label5->Size = System::Drawing::Size(148, 17);
				 this->label5->TabIndex = 24;
				 this->label5->Text = L"IR Cam Address(multi)";
				 // 
				 // txt_IR_ip
				 // 
				 this->txt_IR_ip->Location = System::Drawing::Point(622, 32);
				 this->txt_IR_ip->Name = L"txt_IR_ip";
				 this->txt_IR_ip->Size = System::Drawing::Size(100, 22);
				 this->txt_IR_ip->TabIndex = 25;
				 // 
				 // Btn_Connect_IR
				 // 
				 this->Btn_Connect_IR->Location = System::Drawing::Point(622, 60);
				 this->Btn_Connect_IR->Name = L"Btn_Connect_IR";
				 this->Btn_Connect_IR->Size = System::Drawing::Size(100, 28);
				 this->Btn_Connect_IR->TabIndex = 26;
				 this->Btn_Connect_IR->TabStop = false;
				 this->Btn_Connect_IR->Text = L"Connect Zoom";
				 this->Btn_Connect_IR->UseVisualStyleBackColor = true;
				 this->Btn_Connect_IR->Click += gcnew System::EventHandler(this, &CalibrationUI::Btn_Connect_IR_Click);
				 // 
				 // label6
				 // 
				 this->label6->AutoSize = true;
				 this->label6->Location = System::Drawing::Point(391, 9);
				 this->label6->Name = L"label6";
				 this->label6->Size = System::Drawing::Size(189, 17);
				 this->label6->TabIndex = 27;
				 this->label6->Text = L"Zoom Cam_Control_Address";
				 // 
				 // txt_Zoom_Control_Address
				 // 
				 this->txt_Zoom_Control_Address->Location = System::Drawing::Point(394, 32);
				 this->txt_Zoom_Control_Address->Name = L"txt_Zoom_Control_Address";
				 this->txt_Zoom_Control_Address->Size = System::Drawing::Size(100, 22);
				 this->txt_Zoom_Control_Address->TabIndex = 28;
				 // 
				 // btn_connect_control_zoom
				 // 
				 this->btn_connect_control_zoom->Location = System::Drawing::Point(394, 60);
				 this->btn_connect_control_zoom->Name = L"btn_connect_control_zoom";
				 this->btn_connect_control_zoom->Size = System::Drawing::Size(100, 28);
				 this->btn_connect_control_zoom->TabIndex = 29;
				 this->btn_connect_control_zoom->TabStop = false;
				 this->btn_connect_control_zoom->Text = L"Connect Zoom";
				 this->btn_connect_control_zoom->UseVisualStyleBackColor = true;
				 this->btn_connect_control_zoom->Click += gcnew System::EventHandler(this, &CalibrationUI::btn_connect_control_zoom_Click);
				 // 
				 // label7
				 // 
				 this->label7->AutoSize = true;
				 this->label7->Location = System::Drawing::Point(73, 33);
				 this->label7->Name = L"label7";
				 this->label7->Size = System::Drawing::Size(144, 17);
				 this->label7->TabIndex = 30;
				 this->label7->Text = L"Serial Communication";
				 // 
				 // cbPorts
				 // 
				 this->cbPorts->FormattingEnabled = true;
				 this->cbPorts->Location = System::Drawing::Point(76, 53);
				 this->cbPorts->Name = L"cbPorts";
				 this->cbPorts->Size = System::Drawing::Size(121, 24);
				 this->cbPorts->TabIndex = 31;
				 // 
				 // btnSerialConnect
				 // 
				 this->btnSerialConnect->Location = System::Drawing::Point(76, 83);
				 this->btnSerialConnect->Name = L"btnSerialConnect";
				 this->btnSerialConnect->Size = System::Drawing::Size(178, 28);
				 this->btnSerialConnect->TabIndex = 32;
				 this->btnSerialConnect->Text = L"Connect";
				 this->btnSerialConnect->UseVisualStyleBackColor = true;
				 this->btnSerialConnect->Click += gcnew System::EventHandler(this, &CalibrationUI::btnSerialConnect_Click);
				 // 
				 // btn_start_Signal_Lamp_Calibration
				 // 
				 this->btn_start_Signal_Lamp_Calibration->Location = System::Drawing::Point(90, 221);
				 this->btn_start_Signal_Lamp_Calibration->Name = L"btn_start_Signal_Lamp_Calibration";
				 this->btn_start_Signal_Lamp_Calibration->Size = System::Drawing::Size(178, 28);
				 this->btn_start_Signal_Lamp_Calibration->TabIndex = 66;
				 this->btn_start_Signal_Lamp_Calibration->Text = L"Start_S-lamp_Auto-Aim";
				 this->btn_start_Signal_Lamp_Calibration->UseVisualStyleBackColor = true;
				 this->btn_start_Signal_Lamp_Calibration->Click += gcnew System::EventHandler(this, &CalibrationUI::btn_start_Signal_Lamp_Calibration_Click);
				 // 
				 // lstCalibrationPreview
				 // 
				 this->lstCalibrationPreview->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular,
					 System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
				 this->lstCalibrationPreview->FormattingEnabled = true;
				 this->lstCalibrationPreview->ItemHeight = 25;
				 this->lstCalibrationPreview->Location = System::Drawing::Point(819, 43);
				 this->lstCalibrationPreview->Name = L"lstCalibrationPreview";
				 this->lstCalibrationPreview->Size = System::Drawing::Size(393, 79);
				 this->lstCalibrationPreview->TabIndex = 34;
				 // 
				 // btn_save_pos
				 // 
				 this->btn_save_pos->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 16.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->btn_save_pos->Location = System::Drawing::Point(383, 504);
				 this->btn_save_pos->Name = L"btn_save_pos";
				 this->btn_save_pos->Size = System::Drawing::Size(233, 46);
				 this->btn_save_pos->TabIndex = 35;
				 this->btn_save_pos->Text = L"ADD_DEVICE";
				 this->btn_save_pos->UseVisualStyleBackColor = true;
				 this->btn_save_pos->Click += gcnew System::EventHandler(this, &CalibrationUI::btn_save_pos_Click);
				 // 
				 // label8
				 // 
				 this->label8->AutoSize = true;
				 this->label8->Location = System::Drawing::Point(13, 195);
				 this->label8->Name = L"label8";
				 this->label8->Size = System::Drawing::Size(33, 17);
				 this->label8->TabIndex = 36;
				 this->label8->Text = L"Pan";
				 // 
				 // label9
				 // 
				 this->label9->AutoSize = true;
				 this->label9->Location = System::Drawing::Point(13, 233);
				 this->label9->Name = L"label9";
				 this->label9->Size = System::Drawing::Size(27, 17);
				 this->label9->TabIndex = 37;
				 this->label9->Text = L"Tilt";
				 // 
				 // btn_Delete_Position
				 // 
				 this->btn_Delete_Position->Location = System::Drawing::Point(819, 128);
				 this->btn_Delete_Position->Name = L"btn_Delete_Position";
				 this->btn_Delete_Position->Size = System::Drawing::Size(393, 28);
				 this->btn_Delete_Position->TabIndex = 38;
				 this->btn_Delete_Position->Text = L"Delete Position";
				 this->btn_Delete_Position->UseVisualStyleBackColor = true;
				 this->btn_Delete_Position->Click += gcnew System::EventHandler(this, &CalibrationUI::btn_Delete_Position_Click);
				 // 
				 // label10
				 // 
				 this->label10->AutoSize = true;
				 this->label10->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 16, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->label10->Location = System::Drawing::Point(816, 9);
				 this->label10->Name = L"label10";
				 this->label10->Size = System::Drawing::Size(302, 31);
				 this->label10->TabIndex = 39;
				 this->label10->Text = L"Device Coordinates List";
				 // 
				 // Bar_Resolution
				 // 
				 this->Bar_Resolution->Location = System::Drawing::Point(40, 81);
				 this->Bar_Resolution->Name = L"Bar_Resolution";
				 this->Bar_Resolution->Size = System::Drawing::Size(104, 56);
				 this->Bar_Resolution->TabIndex = 40;
				 this->Bar_Resolution->ValueChanged += gcnew System::EventHandler(this, &CalibrationUI::Bar_Resolution_ValueChanged);
				 // 
				 // txt_Ptz_Resolution
				 // 
				 this->txt_Ptz_Resolution->Enabled = false;
				 this->txt_Ptz_Resolution->Location = System::Drawing::Point(204, 81);
				 this->txt_Ptz_Resolution->Name = L"txt_Ptz_Resolution";
				 this->txt_Ptz_Resolution->Size = System::Drawing::Size(100, 22);
				 this->txt_Ptz_Resolution->TabIndex = 42;
				 // 
				 // btn_digital_zoomOut
				 // 
				 this->btn_digital_zoomOut->Location = System::Drawing::Point(394, 421);
				 this->btn_digital_zoomOut->Name = L"btn_digital_zoomOut";
				 this->btn_digital_zoomOut->Size = System::Drawing::Size(146, 28);
				 this->btn_digital_zoomOut->TabIndex = 43;
				 this->btn_digital_zoomOut->Text = L"Digital_Zoom -";
				 this->btn_digital_zoomOut->UseVisualStyleBackColor = true;
				 this->btn_digital_zoomOut->Click += gcnew System::EventHandler(this, &CalibrationUI::btn_Digital_Zoom_Click);
				 // 
				 // rB_ZoomCam_DZ
				 // 
				 this->rB_ZoomCam_DZ->AutoSize = true;
				 this->rB_ZoomCam_DZ->Location = System::Drawing::Point(394, 321);
				 this->rB_ZoomCam_DZ->Name = L"rB_ZoomCam_DZ";
				 this->rB_ZoomCam_DZ->Size = System::Drawing::Size(157, 21);
				 this->rB_ZoomCam_DZ->TabIndex = 44;
				 this->rB_ZoomCam_DZ->TabStop = true;
				 this->rB_ZoomCam_DZ->Text = L"Zoom Cam Digital_Z";
				 this->rB_ZoomCam_DZ->UseVisualStyleBackColor = true;
				 // 
				 // rB_IR_DZ
				 // 
				 this->rB_IR_DZ->AutoSize = true;
				 this->rB_IR_DZ->Location = System::Drawing::Point(394, 352);
				 this->rB_IR_DZ->Name = L"rB_IR_DZ";
				 this->rB_IR_DZ->Size = System::Drawing::Size(169, 21);
				 this->rB_IR_DZ->TabIndex = 45;
				 this->rB_IR_DZ->TabStop = true;
				 this->rB_IR_DZ->Text = L"IR_Cam_Digital_Zoom";
				 this->rB_IR_DZ->UseVisualStyleBackColor = true;
				 // 
				 // lst_Zoom_box
				 // 
				 this->lst_Zoom_box->FormattingEnabled = true;
				 this->lst_Zoom_box->ItemHeight = 16;
				 this->lst_Zoom_box->Location = System::Drawing::Point(501, 95);
				 this->lst_Zoom_box->Name = L"lst_Zoom_box";
				 this->lst_Zoom_box->Size = System::Drawing::Size(120, 212);
				 this->lst_Zoom_box->TabIndex = 47;
				 this->lst_Zoom_box->SelectedIndexChanged += gcnew System::EventHandler(this, &CalibrationUI::lst_Zoom_box_SelectedIndexChanged);
				 // 
				 // btn_focus_far
				 // 
				 this->btn_focus_far->Location = System::Drawing::Point(394, 164);
				 this->btn_focus_far->Name = L"btn_focus_far";
				 this->btn_focus_far->Size = System::Drawing::Size(100, 28);
				 this->btn_focus_far->TabIndex = 48;
				 this->btn_focus_far->Text = L"Focus_Far";
				 this->btn_focus_far->UseVisualStyleBackColor = true;
				 this->btn_focus_far->Click += gcnew System::EventHandler(this, &CalibrationUI::ZoomCam_Focus_Button_Click);
				 // 
				 // btn_focus_near
				 // 
				 this->btn_focus_near->Location = System::Drawing::Point(394, 198);
				 this->btn_focus_near->Name = L"btn_focus_near";
				 this->btn_focus_near->Size = System::Drawing::Size(100, 28);
				 this->btn_focus_near->TabIndex = 49;
				 this->btn_focus_near->Text = L"Focus_Near";
				 this->btn_focus_near->UseVisualStyleBackColor = true;
				 this->btn_focus_near->Click += gcnew System::EventHandler(this, &CalibrationUI::ZoomCam_Focus_Button_Click);
				 // 
				 // label12
				 // 
				 this->label12->AutoSize = true;
				 this->label12->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 25.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->label12->Location = System::Drawing::Point(829, 324);
				 this->label12->Name = L"label12";
				 this->label12->Size = System::Drawing::Size(108, 51);
				 this->label12->TabIndex = 50;
				 this->label12->Text = L"CH1";
				 // 
				 // label13
				 // 
				 this->label13->AutoSize = true;
				 this->label13->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 25.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->label13->Location = System::Drawing::Point(1007, 324);
				 this->label13->Name = L"label13";
				 this->label13->Size = System::Drawing::Size(108, 51);
				 this->label13->TabIndex = 51;
				 this->label13->Text = L"CH0";
				 // 
				 // lbCh1
				 // 
				 this->lbCh1->AutoSize = true;
				 this->lbCh1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 25.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->lbCh1->Location = System::Drawing::Point(829, 387);
				 this->lbCh1->Name = L"lbCh1";
				 this->lbCh1->Size = System::Drawing::Size(162, 51);
				 this->lbCh1->TabIndex = 52;
				 this->lbCh1->Text = L"label14";
				 // 
				 // lbCh0
				 // 
				 this->lbCh0->AutoSize = true;
				 this->lbCh0->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 25.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->lbCh0->Location = System::Drawing::Point(1007, 387);
				 this->lbCh0->Name = L"lbCh0";
				 this->lbCh0->Size = System::Drawing::Size(162, 51);
				 this->lbCh0->TabIndex = 53;
				 this->lbCh0->Text = L"label15";
				 // 
				 // backgroundWorker1
				 // 
				 this->backgroundWorker1->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &CalibrationUI::backgroundWorker1_GUI_Updater);
				 // 
				 // robotCalibrationThread
				 // 
				 this->robotCalibrationThread->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &CalibrationUI::backgroundWorker2_Signal_Lamp_Calibrator);
				 // 
				 // txt_Setpos_Pan
				 // 
				 this->txt_Setpos_Pan->Location = System::Drawing::Point(123, 393);
				 this->txt_Setpos_Pan->Name = L"txt_Setpos_Pan";
				 this->txt_Setpos_Pan->Size = System::Drawing::Size(100, 22);
				 this->txt_Setpos_Pan->TabIndex = 54;
				 // 
				 // txt_Setpos_Tilt
				 // 
				 this->txt_Setpos_Tilt->Location = System::Drawing::Point(123, 421);
				 this->txt_Setpos_Tilt->Name = L"txt_Setpos_Tilt";
				 this->txt_Setpos_Tilt->Size = System::Drawing::Size(100, 22);
				 this->txt_Setpos_Tilt->TabIndex = 55;
				 // 
				 // label14
				 // 
				 this->label14->AutoSize = true;
				 this->label14->Location = System::Drawing::Point(50, 396);
				 this->label14->Name = L"label14";
				 this->label14->Size = System::Drawing::Size(33, 17);
				 this->label14->TabIndex = 56;
				 this->label14->Text = L"Pan";
				 // 
				 // label15
				 // 
				 this->label15->AutoSize = true;
				 this->label15->Location = System::Drawing::Point(50, 424);
				 this->label15->Name = L"label15";
				 this->label15->Size = System::Drawing::Size(27, 17);
				 this->label15->TabIndex = 57;
				 this->label15->Text = L"Tilt";
				 // 
				 // label16
				 // 
				 this->label16->AutoSize = true;
				 this->label16->Location = System::Drawing::Point(120, 373);
				 this->label16->Name = L"label16";
				 this->label16->Size = System::Drawing::Size(83, 17);
				 this->label16->TabIndex = 58;
				 this->label16->Text = L"Set Position";
				 // 
				 // btn_setPos
				 // 
				 this->btn_setPos->Location = System::Drawing::Point(229, 421);
				 this->btn_setPos->Name = L"btn_setPos";
				 this->btn_setPos->Size = System::Drawing::Size(100, 28);
				 this->btn_setPos->TabIndex = 59;
				 this->btn_setPos->Text = L"apply";
				 this->btn_setPos->UseVisualStyleBackColor = true;
				 this->btn_setPos->Click += gcnew System::EventHandler(this, &CalibrationUI::Button_Click);
				 // 
				 // txt_Pan_step_size
				 // 
				 this->txt_Pan_step_size->Location = System::Drawing::Point(76, 196);
				 this->txt_Pan_step_size->Name = L"txt_Pan_step_size";
				 this->txt_Pan_step_size->Size = System::Drawing::Size(100, 22);
				 this->txt_Pan_step_size->TabIndex = 64;
				 // 
				 // txt_Tilt_step_size
				 // 
				 this->txt_Tilt_step_size->Location = System::Drawing::Point(182, 196);
				 this->txt_Tilt_step_size->Name = L"txt_Tilt_step_size";
				 this->txt_Tilt_step_size->Size = System::Drawing::Size(100, 22);
				 this->txt_Tilt_step_size->TabIndex = 65;
				 // 
				 // label17
				 // 
				 this->label17->AutoSize = true;
				 this->label17->Location = System::Drawing::Point(73, 176);
				 this->label17->Name = L"label17";
				 this->label17->Size = System::Drawing::Size(105, 17);
				 this->label17->TabIndex = 78;
				 this->label17->Text = L"Pan_Step_Size";
				 // 
				 // label18
				 // 
				 this->label18->AutoSize = true;
				 this->label18->Location = System::Drawing::Point(179, 176);
				 this->label18->Name = L"label18";
				 this->label18->Size = System::Drawing::Size(99, 17);
				 this->label18->TabIndex = 77;
				 this->label18->Text = L"Tilt_Step_Size";
				 // 
				 // txt_Row_size
				 // 
				 this->txt_Row_size->Location = System::Drawing::Point(76, 141);
				 this->txt_Row_size->Name = L"txt_Row_size";
				 this->txt_Row_size->Size = System::Drawing::Size(100, 22);
				 this->txt_Row_size->TabIndex = 62;
				 // 
				 // txt_Col_size
				 // 
				 this->txt_Col_size->Location = System::Drawing::Point(182, 141);
				 this->txt_Col_size->Name = L"txt_Col_size";
				 this->txt_Col_size->Size = System::Drawing::Size(100, 22);
				 this->txt_Col_size->TabIndex = 63;
				 // 
				 // label19
				 // 
				 this->label19->AutoSize = true;
				 this->label19->Location = System::Drawing::Point(73, 121);
				 this->label19->Name = L"label19";
				 this->label19->Size = System::Drawing::Size(50, 17);
				 this->label19->TabIndex = 76;
				 this->label19->Text = L"#Rows";
				 // 
				 // label20
				 // 
				 this->label20->AutoSize = true;
				 this->label20->Location = System::Drawing::Point(179, 121);
				 this->label20->Name = L"label20";
				 this->label20->Size = System::Drawing::Size(70, 17);
				 this->label20->TabIndex = 75;
				 this->label20->Text = L"#Columns";
				 // 
				 // lbl_pan_arcsec
				 // 
				 this->lbl_pan_arcsec->AutoSize = true;
				 this->lbl_pan_arcsec->Location = System::Drawing::Point(120, 62);
				 this->lbl_pan_arcsec->Name = L"lbl_pan_arcsec";
				 this->lbl_pan_arcsec->Size = System::Drawing::Size(50, 17);
				 this->lbl_pan_arcsec->TabIndex = 68;
				 this->lbl_pan_arcsec->Text = L"arcsec";
				 // 
				 // lbl_tilt_arcsec
				 // 
				 this->lbl_tilt_arcsec->AutoSize = true;
				 this->lbl_tilt_arcsec->Location = System::Drawing::Point(6, 62);
				 this->lbl_tilt_arcsec->Name = L"lbl_tilt_arcsec";
				 this->lbl_tilt_arcsec->Size = System::Drawing::Size(108, 17);
				 this->lbl_tilt_arcsec->TabIndex = 69;
				 this->lbl_tilt_arcsec->Text = L"arcsec setting =";
				 // 
				 // label21
				 // 
				 this->label21->AutoSize = true;
				 this->label21->Location = System::Drawing::Point(201, 62);
				 this->label21->Name = L"label21";
				 this->label21->Size = System::Drawing::Size(72, 17);
				 this->label21->TabIndex = 70;
				 this->label21->Text = L"Step_Size";
				 // 
				 // label22
				 // 
				 this->label22->AutoSize = true;
				 this->label22->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 25.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->label22->Location = System::Drawing::Point(810, 170);
				 this->label22->Name = L"label22";
				 this->label22->Size = System::Drawing::Size(446, 51);
				 this->label22->TabIndex = 71;
				 this->label22->Text = L"Offset from ZoomCam";
				 // 
				 // lbl_offset_tilt
				 // 
				 this->lbl_offset_tilt->AutoSize = true;
				 this->lbl_offset_tilt->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 25.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->lbl_offset_tilt->Location = System::Drawing::Point(1069, 228);
				 this->lbl_offset_tilt->Name = L"lbl_offset_tilt";
				 this->lbl_offset_tilt->Size = System::Drawing::Size(46, 51);
				 this->lbl_offset_tilt->TabIndex = 72;
				 this->lbl_offset_tilt->Text = L"_";
				 // 
				 // lbl_offset_pan
				 // 
				 this->lbl_offset_pan->AutoSize = true;
				 this->lbl_offset_pan->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 25.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->lbl_offset_pan->Location = System::Drawing::Point(834, 228);
				 this->lbl_offset_pan->Name = L"lbl_offset_pan";
				 this->lbl_offset_pan->Size = System::Drawing::Size(46, 51);
				 this->lbl_offset_pan->TabIndex = 73;
				 this->lbl_offset_pan->Text = L"_";
				 // 
				 // btn_Grid
				 // 
				 this->btn_Grid->Location = System::Drawing::Point(68, 89);
				 this->btn_Grid->Name = L"btn_Grid";
				 this->btn_Grid->Size = System::Drawing::Size(96, 22);
				 this->btn_Grid->TabIndex = 74;
				 this->btn_Grid->Text = L"Grid On/Off";
				 this->btn_Grid->UseVisualStyleBackColor = true;
				 this->btn_Grid->Click += gcnew System::EventHandler(this, &CalibrationUI::btn_Grid_Click);
				 // 
				 // groupBox1
				 // 
				 this->groupBox1->Controls->Add(this->txt_Col_size);
				 this->groupBox1->Controls->Add(this->btn_start_Signal_Lamp_Calibration);
				 this->groupBox1->Controls->Add(this->txt_Pan_step_size);
				 this->groupBox1->Controls->Add(this->txt_Tilt_step_size);
				 this->groupBox1->Controls->Add(this->label17);
				 this->groupBox1->Controls->Add(this->label18);
				 this->groupBox1->Controls->Add(this->txt_Row_size);
				 this->groupBox1->Controls->Add(this->label19);
				 this->groupBox1->Controls->Add(this->label20);
				 this->groupBox1->Controls->Add(this->label7);
				 this->groupBox1->Controls->Add(this->cbPorts);
				 this->groupBox1->Controls->Add(this->btnSerialConnect);
				 this->groupBox1->Location = System::Drawing::Point(622, 453);
				 this->groupBox1->Name = L"groupBox1";
				 this->groupBox1->Size = System::Drawing::Size(315, 263);
				 this->groupBox1->TabIndex = 79;
				 this->groupBox1->TabStop = false;
				 this->groupBox1->Text = L"Auto_Signal_Lamp_Calibration";
				 // 
				 // btn_goto_grid
				 // 
				 this->btn_goto_grid->Location = System::Drawing::Point(31, 167);
				 this->btn_goto_grid->Name = L"btn_goto_grid";
				 this->btn_goto_grid->Size = System::Drawing::Size(178, 28);
				 this->btn_goto_grid->TabIndex = 69;
				 this->btn_goto_grid->Text = L"go_to_col_row";
				 this->btn_goto_grid->UseVisualStyleBackColor = true;
				 this->btn_goto_grid->Click += gcnew System::EventHandler(this, &CalibrationUI::btn_goto_grid_Click);
				 // 
				 // label24
				 // 
				 this->label24->AutoSize = true;
				 this->label24->Location = System::Drawing::Point(150, 122);
				 this->label24->Name = L"label24";
				 this->label24->Size = System::Drawing::Size(35, 17);
				 this->label24->TabIndex = 82;
				 this->label24->Text = L"Row";
				 // 
				 // txt_row_nr
				 // 
				 this->txt_row_nr->Location = System::Drawing::Point(120, 141);
				 this->txt_row_nr->Name = L"txt_row_nr";
				 this->txt_row_nr->Size = System::Drawing::Size(100, 22);
				 this->txt_row_nr->TabIndex = 68;
				 // 
				 // txt_col_nr
				 // 
				 this->txt_col_nr->Location = System::Drawing::Point(14, 141);
				 this->txt_col_nr->Name = L"txt_col_nr";
				 this->txt_col_nr->Size = System::Drawing::Size(100, 22);
				 this->txt_col_nr->TabIndex = 67;
				 // 
				 // label23
				 // 
				 this->label23->AutoSize = true;
				 this->label23->Location = System::Drawing::Point(45, 121);
				 this->label23->Name = L"label23";
				 this->label23->Size = System::Drawing::Size(28, 17);
				 this->label23->TabIndex = 79;
				 this->label23->Text = L"Col";
				 // 
				 // groupBox2
				 // 
				 this->groupBox2->Controls->Add(this->label23);
				 this->groupBox2->Controls->Add(this->btn_goto_grid);
				 this->groupBox2->Controls->Add(this->txt_col_nr);
				 this->groupBox2->Controls->Add(this->txt_row_nr);
				 this->groupBox2->Controls->Add(this->label24);
				 this->groupBox2->Controls->Add(this->btn_Grid);
				 this->groupBox2->Location = System::Drawing::Point(968, 453);
				 this->groupBox2->Name = L"groupBox2";
				 this->groupBox2->Size = System::Drawing::Size(244, 263);
				 this->groupBox2->TabIndex = 83;
				 this->groupBox2->TabStop = false;
				 this->groupBox2->Text = L"Experimental Tools";
				 // 
				 // groupBox3
				 // 
				 this->groupBox3->Controls->Add(this->label9);
				 this->groupBox3->Controls->Add(this->label8);
				 this->groupBox3->Controls->Add(this->tiltTxt);
				 this->groupBox3->Controls->Add(this->panTxt);
				 this->groupBox3->Controls->Add(this->txtPanDeg);
				 this->groupBox3->Controls->Add(this->btn_setPos);
				 this->groupBox3->Controls->Add(this->label16);
				 this->groupBox3->Controls->Add(this->label21);
				 this->groupBox3->Controls->Add(this->label15);
				 this->groupBox3->Controls->Add(this->lbl_tilt_arcsec);
				 this->groupBox3->Controls->Add(this->label14);
				 this->groupBox3->Controls->Add(this->txtTiltDeg);
				 this->groupBox3->Controls->Add(this->txt_Setpos_Tilt);
				 this->groupBox3->Controls->Add(this->lbl_pan_arcsec);
				 this->groupBox3->Controls->Add(this->txt_Setpos_Pan);
				 this->groupBox3->Controls->Add(this->txt_Ptz_Resolution);
				 this->groupBox3->Controls->Add(this->Bar_Resolution);
				 this->groupBox3->Controls->Add(this->rstButton);
				 this->groupBox3->Controls->Add(this->leftbtn);
				 this->groupBox3->Controls->Add(this->rightbtn);
				 this->groupBox3->Controls->Add(this->upBtn);
				 this->groupBox3->Controls->Add(this->downBtn);
				 this->groupBox3->Controls->Add(this->label4);
				 this->groupBox3->Controls->Add(this->label3);
				 this->groupBox3->Location = System::Drawing::Point(5, 198);
				 this->groupBox3->Name = L"groupBox3";
				 this->groupBox3->Size = System::Drawing::Size(372, 521);
				 this->groupBox3->TabIndex = 84;
				 this->groupBox3->TabStop = false;
				 this->groupBox3->Text = L"Robot Control";
				 // 
				 // label11
				 // 
				 this->label11->AutoSize = true;
				 this->label11->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->label11->Location = System::Drawing::Point(1262, 170);
				 this->label11->Name = L"label11";
				 this->label11->Size = System::Drawing::Size(337, 40);
				 this->label11->TabIndex = 85;
				 this->label11->Text = L"+Pan moves robot in the right direction\r\n+Tilt  moves robot upwards";
				 // 
				 // CalibrationUI
				 // 
				 this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
				 this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				 this->ClientSize = System::Drawing::Size(1610, 745);
				 this->Controls->Add(this->label11);
				 this->Controls->Add(this->groupBox2);
				 this->Controls->Add(this->groupBox1);
				 this->Controls->Add(this->lbl_offset_pan);
				 this->Controls->Add(this->lbl_offset_tilt);
				 this->Controls->Add(this->label22);
				 this->Controls->Add(this->lbCh0);
				 this->Controls->Add(this->lbCh1);
				 this->Controls->Add(this->label13);
				 this->Controls->Add(this->label12);
				 this->Controls->Add(this->btn_focus_near);
				 this->Controls->Add(this->btn_focus_far);
				 this->Controls->Add(this->lst_Zoom_box);
				 this->Controls->Add(this->rB_IR_DZ);
				 this->Controls->Add(this->rB_ZoomCam_DZ);
				 this->Controls->Add(this->btn_digital_zoomOut);
				 this->Controls->Add(this->label10);
				 this->Controls->Add(this->btn_Delete_Position);
				 this->Controls->Add(this->btn_save_pos);
				 this->Controls->Add(this->lstCalibrationPreview);
				 this->Controls->Add(this->btn_connect_control_zoom);
				 this->Controls->Add(this->txt_Zoom_Control_Address);
				 this->Controls->Add(this->label6);
				 this->Controls->Add(this->Btn_Connect_IR);
				 this->Controls->Add(this->txt_IR_ip);
				 this->Controls->Add(this->label5);
				 this->Controls->Add(this->btn_manual_focus);
				 this->Controls->Add(this->btn_digital_zoomIn);
				 this->Controls->Add(this->btn_IRCam_Switch);
				 this->Controls->Add(this->btn_auto_focus);
				 this->Controls->Add(this->Btn_Connect_Zoom);
				 this->Controls->Add(this->txt_Zoom_ip);
				 this->Controls->Add(this->label2);
				 this->Controls->Add(this->label1);
				 this->Controls->Add(this->Btn_Connect_Ptz);
				 this->Controls->Add(this->txt_PTZ_ip);
				 this->Controls->Add(this->btnRobotTst);
				 this->Controls->Add(this->btn_ZoomCam_Switch);
				 this->Controls->Add(this->groupBox3);
				 this->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
				 this->Name = L"CalibrationUI";
				 this->Text = L"PTZ Calibration Tool";
				 this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &CalibrationUI::CalibrationUI_KeyDown);
				 (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->Bar_Resolution))->EndInit();
				 this->groupBox1->ResumeLayout(false);
				 this->groupBox1->PerformLayout();
				 this->groupBox2->ResumeLayout(false);
				 this->groupBox2->PerformLayout();
				 this->groupBox3->ResumeLayout(false);
				 this->groupBox3->PerformLayout();
				 this->ResumeLayout(false);
				 this->PerformLayout();

			 }
#pragma endregion

			 //*============***************===========================Event Handlers===============================**************============================*/
	private: System::Void Zoom_Cam_Click(System::Object^ sender, System::EventArgs^ e)
	{
		bool ZoomCam_Active = myVideoStreamerObject->get_Zoom_Flag();
		ZoomCam_Active = !ZoomCam_Active;
		myVideoStreamerObject->set_Zoom_Flag(ZoomCam_Active);
	}
	private: System::Void btn_Grid_Click(System::Object^  sender, System::EventArgs^  e)
	{
		bool Grid_Active = myVideoStreamerObject->get_Grid_Flag();
		Grid_Active = !Grid_Active;
		myVideoStreamerObject->set_Grid_Flag(Grid_Active);

		if (Grid_Active)
		{
			int Rows_limit;//10;//set from UI
			int Col_limit;// = 10;
			int step_size_pan;// = 50;//default
			int step_size_tilt;// = 10;

			try//read from input
			{
				get_Automation_Variables(&Rows_limit, &Col_limit, &step_size_pan, &step_size_tilt);
				myVideoStreamerObject->setGridDimensions(Col_limit, Rows_limit, step_size_pan, step_size_tilt);
				//center the grid
				myVideoStreamerObject->Set_Grid_Reference_Axis(0, 0);
			}
			catch (...)
			{

			}
		}

	}
	private: System::Void IR_Cam_Click(System::Object^  sender, System::EventArgs^  e)
	{
		bool IRCam_Active = myVideoStreamerObject->get_IR_Flag();
		IRCam_Active = !IRCam_Active;
		myVideoStreamerObject->set_IR_Status_Flag(IRCam_Active);
	}
	private: System::Void btnRobotTst_Click(System::Object^ sender, System::EventArgs^ e) {

		struct cerial *cer;
		uint16_t status;
		int pn, px, tn, tx, pu, tu;
		int i = 0;
		int argc = 3;

		char *param1 = "CalibrationApp.exe";
		char *param2 = "-p";
		char *undefined_tcp = "tcp:";

		//make a try catch block here because of txtb input
		string ip_address = "";
		System::String^ managed;
		managed = txt_PTZ_ip->Text;
		ip_address = msclr::interop::marshal_as<std::string>(managed);

		string string_ipaddress = undefined_tcp + ip_address;

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

		cout << "this is whats passed" << endl;
		cout << command_line[0];
		cout << command_line[1];
		cout << command_line[2] << endl;

		/*
		format[0] = "monitor.exe";
		format[1] = "-p";
		format[2] = "tcp:10.10.1.90";
		*/

		//fan den kunde ju ta string direkt!! byt ut command_line mot  string command_line

		if ((cer = estrap_in(argc, command_line)) == NULL)
		{
			*command_line = NULL;
			cout << "**Connection failed**" << endl;
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

			cout << ("\n 1 \n");
			/* cycle twice between min and max positions */
			ptu_go_to(cer, pn, tn);
			ptu_go_to(cer, px, tx);
			ptu_go_to(cer, pn, tn);
			ptu_go_to(cer, px, tx);

			cout << ("2 \n");

			/* cycle between pre-defined positions */
			pn = -5000;
			px = 5000;
			tn = -1000;
			tx = 1000;

			cout << ("iterating \n");

			for (i = 0; i < NR_ITERATIONS; i++)
			{
				ptu_go_to(cer, pn, tn);
				ptu_go_to(cer, px, tx);
			}
			/* and go home */
			ptu_go_to(cer, 0, 0);

			estrap_out(cer);

			printf("slut \n");
		}
	}
	private: System::Void Button_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if (sender == upBtn)
		{
			cout << "up" << endl;
			tilt += PTZ_stepSize;
			myRobot->set_pos(pan, tilt);//pan + går up
		}
		else if (sender == downBtn)
		{
			cout << "down" << endl;
			tilt -= PTZ_stepSize;
			myRobot->set_pos(pan, tilt);//pan - går ner
		}
		else if (sender == leftbtn)
		{
			cout << "left" << endl;
			pan -= PTZ_stepSize;//step size //for small change to 10
			myRobot->set_pos(pan, tilt);//pan - går till vänster
		}
		else if (sender == rightbtn)
		{
			cout << "right" << endl;
			pan += PTZ_stepSize;
			myRobot->set_pos(pan, tilt);//pan + går till vänster
		}
		else if (sender == rstButton)
		{
			myRobot->Reset();
			pan = 0;
			tilt = 0;
		}
		else if (sender == btn_setPos)
		{

			try
			{
				std::string string_pan = msclr::interop::marshal_as<std::string>(txt_Setpos_Pan->Text);
				std::string string_tilt = msclr::interop::marshal_as<std::string>(txt_Setpos_Tilt->Text);

				pan = std::stoi(string_pan);
				tilt = stoi(string_tilt);
				myRobot->set_pos(pan, tilt);
				if (myVideoStreamerObject->Grid_Active)
				{
					//center the grid
					myVideoStreamerObject->Set_Grid_Reference_Axis(0, 0);
				}
			}
			catch (...)
			{
			}

		}

		Populate_Pan_Tilt_Info();
	}
	private: System::Void PTZ_connectBtn_Click(System::Object^  sender, System::EventArgs^  e)
	{
		try
		{
			myRobot->Connect("10.10.1.90");
		}
		catch (...)
		{

		}
		resolution_pan = (double)myRobot->get_Pan_Resolution() / (double)3600.0;//init resolution
		resolution_tilt = (double)myRobot->get_Tilt_Resolution() / (double)3600.0;
		lbl_pan_arcsec->Text = myRobot->get_Pan_Resolution().ToString();

		Populate_Pan_Tilt_Info();
		txt_Ptz_Resolution->Text = PTZ_stepSize.ToString();
	}
	private: System::Void CalibrationUI_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e)
	{
		if (e->KeyCode == Keys::NumPad4)
		{
			Button_Click(leftbtn, e);
		}
		else if (e->KeyCode == Keys::NumPad6)
		{
			Button_Click(rightbtn, e);
		}
		else if (e->KeyCode == Keys::NumPad8)
		{
			Button_Click(upBtn, e);
		}
		else if (e->KeyCode == Keys::NumPad2)
		{
			Button_Click(downBtn, e);
		}
	}
	private: System::Void ZoomCam_Focus_Button_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if (sender == btn_auto_focus)
		{
			ZoomClient->Send(buffer_AutoFocusOn);
		}
		else if (sender == btn_manual_focus)
		{
			ZoomClient->Send(buffer_AutoFocusOff);
		}
		else if (sender == btn_focus_far)
		{
			ZoomClient->Send16(buffer_FocusFar);
			Thread::Sleep(100);
			ZoomClient->Send16(buffer_FocusStop);
			cout << "stopped focus far" << std::endl;
		}
		else if (sender == btn_focus_near)
		{
			ZoomClient->Send16(buffer_FocusNear);
			Thread::Sleep(100);
			ZoomClient->Send16(buffer_FocusStop);
			cout << "stopped focus near" << std::endl;
		}
	}
	private: System::Void Zoom_Connect_Click(System::Object^  sender, System::EventArgs^  e)
	{
		//===================================Init Zoom Cam Video connection==============================================//
		System::String^ protocol = "rtp://@";
		System::String^ port = ":22144";

		System::String^ managed_system_string;
		System::String^ user_input_ip;
		user_input_ip = txt_Zoom_ip->Text;

		managed_system_string = protocol + user_input_ip + port;

		ZoomCamThread->Start(managed_system_string);
	}
	private: System::Void btn_connect_control_zoom_Click(System::Object^  sender, System::EventArgs^  e)
	{
		//=============================Initialize Zoom Cam Controls communication API socket==============================//
		System::String^ user_input_ip = txt_Zoom_Control_Address->Text;
		std::string ip_address = msclr::interop::marshal_as<std::string>(user_input_ip);

		char* tmp = "10.10.0.91";

		ZoomClient->Connect(tmp);//replace this with ip_address

		if (!ZoomClient->Start())
			cout << "Zoom cam API communication failed" << endl;
		else
		{
			ZoomClient->Send(buffer_AutoFocusOff);//initialize with autofocus ON
			ZoomClient->Send(buffer_zoomout);
		}

	}
	private: System::Void Btn_Connect_IR_Click(System::Object^  sender, System::EventArgs^  e)
	{
		System::String^ protocol = "rtp://@";
		System::String^ port = ":10000";
		//=======================================Init Zoom IR Video connection==============================================//
		System::String^ managed_system_string;
		System::String^ user_input_ip;
		user_input_ip = txt_IR_ip->Text;

		managed_system_string = protocol + user_input_ip + port;

		IRCamThread->Start(managed_system_string);
	}

	private: System::Void btn_Digital_Zoom_Click(System::Object^  sender, System::EventArgs^  e)
	{
		int zoomlevel;

		if (rB_ZoomCam_DZ->Checked)
		{
			zoomlevel = myVideoStreamerObject->D_ZoomLevel_Zoom_Cam;

			if (sender == btn_digital_zoomIn)
			{
				zoomlevel++;
				myVideoStreamerObject->D_ZoomLevel_Zoom_Cam = zoomlevel;
			}
			else if (sender == btn_digital_zoomOut)
			{
				if (zoomlevel > 1)
				{
					zoomlevel--;
				}
				myVideoStreamerObject->D_ZoomLevel_Zoom_Cam = zoomlevel;
				cout << zoomlevel << endl;
			}
		}
		else if (rB_IR_DZ->Checked)
		{
			zoomlevel = myVideoStreamerObject->D_ZoomLevel_IR_Cam;

			if (sender == btn_digital_zoomIn)
			{
				zoomlevel++;
				myVideoStreamerObject->D_ZoomLevel_IR_Cam = zoomlevel;
				cout << zoomlevel << endl;
			}
			else if (sender == btn_digital_zoomOut)
			{
				if (zoomlevel > 1)
				{
					zoomlevel--;
				}
				myVideoStreamerObject->D_ZoomLevel_IR_Cam = zoomlevel;
				cout << zoomlevel << endl;
			}
		}

	}
	private: System::Void btnSerialConnect_Click(System::Object^  sender, System::EventArgs^  e)
	{
		Console::WriteLine(cbPorts->SelectedItem->ToString());

		System::String ^selectedPort = cbPorts->SelectedItem->ToString();
		mySerial->Initialization(selectedPort);

		readThread->Start();

	}
			 void populateComboBoxSerialPorts()
			 {
				 array<System::String^>^ managedArray = gcnew array<System::String^>(10);

				 managedArray = mySerial->getSerialPorts();

				 int i = 0;

				 while (managedArray[i] != "")
				 {
					 cbPorts->Items->Add(managedArray[i]);
					 i++;
				 }
			 }
			 void populateZoomBox()
			 {
				 for (int i = 1; i < 31; i++)
				 {
					 string item = "Zoom" + std::to_string(i);
					 System::String^ res = gcnew System::String(item.c_str());
					 lst_Zoom_box->Items->Add(res);
				 }

			 }
			 void Populate_Pan_Tilt_Info()
			 {
				 panTxt->Text = Convert::ToString(myRobot->get_pan());//get relative pan
				 tiltTxt->Text = Convert::ToString(myRobot->get_tilt());//get relative tilt

				 txtPanDeg->Text = Convert::ToString((double)myRobot->get_pan() * resolution_pan);//convert to pan deg
				 txtTiltDeg->Text = Convert::ToString((double)myRobot->get_tilt() * resolution_tilt);//convert to tilt deg
			 }
	private: System::Void btn_save_pos_Click(System::Object^  sender, System::EventArgs^  e)
	{
		Add_Position_Dialog_Box ^testDialog = gcnew Add_Position_Dialog_Box(this);

		System::Windows::Forms::DialogResult dr;

		dr = testDialog->ShowDialog();

		System::String ^item = "";
		if (testDialog->getDeviceType() == "zoom")
		{
			ref_device_data->device_type = "ZoomCam";
			ref_device_data->pan = pan;
			ref_device_data->tilt = tilt;
			ref_device_data->pan_deg = pan * resolution_pan;
			ref_device_data->tilt_deg = tilt * resolution_tilt;

			item = ref_device_data->device_type->PadRight(20 - ref_device_data->device_type->Length) + "\t" + "p= " + ref_device_data->pan_deg.ToString()->PadRight(5 - ref_device_data->pan_deg.ToString()->Length) + "\t" + "t= " + ref_device_data->tilt_deg;
			Console::WriteLine("added zoom");
		}
		else if (testDialog->getDeviceType() == "ir" && lstCalibrationPreview->Items->Count > 0)//ZoomCam must be added first
		{
			device_data->device_type = "IR CAM";
			device_data->pan = pan;
			device_data->tilt = tilt;
			device_data->pan_deg = pan * resolution_pan;
			device_data->tilt_deg = tilt * resolution_tilt;

			item = device_data->device_type->PadRight(20 - device_data->device_type->Length) + "\t" + "p= " + device_data->pan_deg.ToString()->PadRight(5 - device_data->pan_deg.ToString()->Length) + "\t" + "t= " + device_data->tilt_deg;
			Console::WriteLine("added ir");
		}
		else if (testDialog->getDeviceType() == "signal" && lstCalibrationPreview->Items->Count > 0)//ZoomCam must be added first
		{
			device_data->device_type = "S-Lamp";
			device_data->pan = pan;
			device_data->tilt = tilt;
			device_data->pan_deg = pan * resolution_pan;
			device_data->tilt_deg = tilt * resolution_tilt;

			item = device_data->device_type->PadRight(20 - device_data->device_type->Length) + "\t" + "p= " + device_data->pan_deg.ToString()->PadRight(5 - device_data->pan_deg.ToString()->Length) + "\t" + "t= " + device_data->tilt_deg;
			Console::WriteLine("added signal");
		}
		if (item != "")
		{
			if (lstCalibrationPreview->Items->Count < 2)//dont add more than two devices
			{
				lstCalibrationPreview->Items->Add(item);
			}
			if (lstCalibrationPreview->Items->Count > 1)//calculate offset when second device is added
			{
				double pan_offset = (device_data->pan_deg - ref_device_data->pan_deg);
				double tilt_offset = (device_data->tilt_deg - ref_device_data->tilt_deg);
				lbl_offset_pan->Text = "p= " + Math::Round(pan_offset,3).ToString() + "°";
				lbl_offset_tilt->Text = "t= " + Math::Round(tilt_offset,3).ToString() + "°";
			}
		}
		else
		{
			MessageBox::Show("Reference cam must be of type zoom");
		}
		delete testDialog;
	}
	private: System::Void btn_Delete_Position_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if (lstCalibrationPreview->SelectedItem->ToString()->Substring(0, 7) == ref_device_data->device_type)//if delete zoomcam delete everything else
		{
			lstCalibrationPreview->Items->Clear();
		}
		else
		{
			lstCalibrationPreview->Items->Remove(lstCalibrationPreview->SelectedItem);
		}
		try
		{
			if (Signal_Data_Exist)
			{
				//allocate dynamic array memory
				for (int j = 0; j < free_nr_rows; j++)
				{
					delete[] signal_lamp_data[j];
				}
				cout << "freed memory" << endl;
				Signal_Data_Exist = false;//reset flag
			}
			else
			{
				cout << "nothing to destroy" << endl;
			}
		}
		finally
		{
			btn_goto_grid->Enabled = false;
			lbl_offset_tilt->Text = "";
			lbl_offset_pan->Text = "";
		}

	}
	private: System::Void Bar_Resolution_ValueChanged(System::Object^  sender, System::EventArgs^  e)
	{
		if (Bar_Resolution->Value == 0)
		{
			PTZ_stepSize = 10;
		}
		else if (Bar_Resolution->Value == 1)
		{
			PTZ_stepSize = 20;
		}
		else if (Bar_Resolution->Value == 2)
		{
			PTZ_stepSize = 30;
		}
		else if (Bar_Resolution->Value == 3)
		{
			PTZ_stepSize = 40;
		}
		else if (Bar_Resolution->Value == 4)
		{
			PTZ_stepSize = 50;
		}
		else if (Bar_Resolution->Value == 5)
		{
			PTZ_stepSize = 60;
		}
		else if (Bar_Resolution->Value == 6)
		{
			PTZ_stepSize = 70;
		}
		else if (Bar_Resolution->Value == 7)
		{
			PTZ_stepSize = 80;
		}
		else if (Bar_Resolution->Value == 8)
		{
			PTZ_stepSize = 90;
		}
		else if (Bar_Resolution->Value == 9)
		{
			PTZ_stepSize = 100;
		}
		txt_Ptz_Resolution->Text = PTZ_stepSize.ToString();
		System::Console::WriteLine(PTZ_stepSize);
	}
	private: System::Void lst_Zoom_box_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
	{
		//System::Console::WriteLine(lst_Zoom_box->SelectedItem->ToString());
		System::String ^selected_item = lst_Zoom_box->SelectedItem->ToString();

		std::string unmanaged = msclr::interop::marshal_as<std::string>(selected_item);

		memset(optical_zoom_level, 0, sizeof(optical_zoom_level));

		ZoomClient->getZoom(optical_zoom_level, unmanaged);

		for (int i = 0; i < 17; i++)
		{
			cout << optical_zoom_level[i];
		}
		cout << endl;
		ZoomClient->Send(optical_zoom_level);//send new zoom level through API

		int o_zoom_level = stoi(unmanaged.substr(4, 2));//scale Grid after optical Zoom
		myVideoStreamerObject->set_O_Zoom_Level(o_zoom_level);
	}

	private: System::Void backgroundWorker1_GUI_Updater(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e)
	{
		System::String ^tmp;
		BackgroundWorker ^worker = (BackgroundWorker^)sender;
		while (1)//(!worker->CancellationPending)
		{
			if (mySerial->getSerial_Connection_Status() == true)
			{
				try//this will always fail until serial connection has been established
				{
					UpdateGUI(mySerial->getCh0(), mySerial->getCh1(), pan, tilt);
					updateAutomationValues();
				}
				catch (...)
				{

				}
			}
		}
	}
			 void updateAutomationValues()
			 {
				 Str_ch0 = msclr::interop::marshal_as<std::string>(mySerial->getCh0());
				 Str_ch1 = msclr::interop::marshal_as<std::string>(mySerial->getCh1());
				 Int_ch0 = stoi(Str_ch0);
				 Int_ch1 = stoi(Str_ch1);
			 }

			 void get_Automation_Variables(int *Rows_limit, int *Col_limit, int *step_size_pan, int *step_size_tilt)
			 {
				 try
				 {
					 std::string string_row_size = msclr::interop::marshal_as<std::string>(txt_Row_size->Text);
					 *Rows_limit = std::stoi(string_row_size);

					 std::string string_col_size = msclr::interop::marshal_as<std::string>(txt_Col_size->Text);
					 *Col_limit = std::stoi(string_col_size);

					 std::string string_step_size_pan = msclr::interop::marshal_as<std::string>(txt_Pan_step_size->Text);
					 *step_size_pan = std::stoi(string_step_size_pan);

					 std::string string_step_size_tilt = msclr::interop::marshal_as<std::string>(txt_Tilt_step_size->Text);
					 *step_size_tilt = std::stoi(string_step_size_tilt);
				 }
				 catch (...)
				 {
					 MessageBox::Show("All Grid Values Must Be Defined");
					 return;
				 }
			 }

	private: System::Void backgroundWorker2_Signal_Lamp_Calibrator(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e)
	{
		int Rows_limit;//10;//set from UI
		int Col_limit;// = 10;
		int step_size_pan;// = 50;//default
		int step_size_tilt;// = 10;

		get_Automation_Variables(&Rows_limit, &Col_limit, &step_size_pan, &step_size_tilt);

		int pan_ref =  ref_device_data->pan -(step_size_pan * Col_limit) / 2;//-2990;
		int tilt_ref = ref_device_data->tilt -(step_size_tilt * Rows_limit ) / 2;//-20;

		//allocate dynamic array memory
		for (int j = 0; j < Rows_limit; j++)
		{
			signal_lamp_data[j] = new SignalLamp_Container[Col_limit];
		}

		while (1)//nr of iterations
		{
			pan = pan_ref;
			tilt = tilt_ref;
			cout << "robot init coordinates: " << "ref pan=" << pan_ref << "	" << "ref tilt=" << tilt_ref << endl;//init line
			myRobot->set_pos(pan_ref, tilt_ref);

			for (int row = 0; row < Rows_limit; row++)
			{
				for (int col = 0; col < Col_limit; col++)
				{
					myVideoStreamerObject->Set_Grid_Reference_Axis(col, row);
					pan = pan_ref + step_size_pan * col;
					tilt = tilt_ref + step_size_tilt * row;
					myRobot->set_pos(pan, tilt);//blocking command move
					cout << Int_ch0 << "	" << Int_ch1 << "	current_pan=" << pan << "	current_tilt=" << tilt << std::endl;

					signal_lamp_data[row][col].pan = pan;
					signal_lamp_data[row][col].tilt = tilt;
					signal_lamp_data[row][col].lux_ch0 = Int_ch0;//just make += for several iterations
					signal_lamp_data[row][col].lux_ch1 = Int_ch1;
					signal_lamp_data[row][col].col = col;
					signal_lamp_data[row][col].row = row;
				}
			}
			//calculate avarege here if there are multiple iterations
			break;
		}
		print2Darray(Rows_limit, Col_limit, step_size_pan, step_size_tilt);
		Signal_Data_Exist = true;//Indicate 2d-matrix has data
		free_nr_rows = Rows_limit;//Nr of data rows that can be freed
		//interpolation();
		calculateOffset_Signal_Lamp(Rows_limit, Col_limit);
	}
			 void print2Darray(int Row, int Col, int pan_step_size, int tilt_step_size)
			 {
				 ofstream myfile;
				 myfile.open("C:\\Users\\u030373\\Desktop\\signal_lamp_data.csv");
				 cout << "printing memory 2D array" << endl;

				 myfile << "Array pos;Pan;Tilt;Channel0;Channel1;Dimensions;Pan_Step_Size;Tilt_Step_Size" << endl;;
				 for (int j = 0; j < Row; j++)
				 {
					 for (int i = 0; i < Col; i++)
					 {
						 cout << "(" << j << "," << i << ")" << "=" << "{" << signal_lamp_data[j][i].pan << "," << signal_lamp_data[j][i].tilt << "}" <<
							 "		" << "ch0= " << signal_lamp_data[j][i].lux_ch0 << " " << "ch1= " << signal_lamp_data[j][i].lux_ch1 << endl;

						 myfile << "(" << j << ",," << i << ")" << ";" << signal_lamp_data[j][i].pan << ";" << signal_lamp_data[j][i].tilt <<
							 ";" << signal_lamp_data[j][i].lux_ch0 << ";" << signal_lamp_data[j][i].lux_ch1 <<
							 ";" << Col << "x" << Row << ";" << pan_step_size << ";" << tilt_step_size <<
							 endl;
					 }
				 }
				 myfile.close();
			 }
			 /*
			 void interpolation()
			 {

			 }
			 */
			 void calculateOffset_Signal_Lamp(int Rows_limit, int Col_limit)
			 {
				 int max = 0;
				 int temp = 0;
				 int m_pan, m_tilt;
				 int m_col, m_row;

				 for (int row = 0; row < Rows_limit; row++)
				 {
					 for (int col = 0; col < Col_limit; col++)
					 {
						 temp = signal_lamp_data[row][col].lux_ch0;
						 if (max < temp)
						 {
							 max = temp;
							 m_pan = signal_lamp_data[row][col].pan;
							 m_tilt = signal_lamp_data[row][col].tilt;
							 m_col = signal_lamp_data[row][col].col;
							 m_row = signal_lamp_data[row][col].row;
						 }
					 }
				 }
				 cout << "max values for signal lamp were located at " << m_pan << "\t" << m_tilt << endl;
				 myRobot->set_pos(m_pan, m_tilt);//blocking command move
				 myVideoStreamerObject->Set_Grid_Reference_Axis(m_col,m_row);

				 double m_pan_deg = (double)m_pan * resolution_pan;
				 double m_tilt_deg = (double)m_tilt * resolution_tilt;
				 
				 System::String ^device = "S-lamp";
				 UpdateGUI2(device,m_pan_deg,m_tilt_deg);//Update Listbox with the new added signal lamp
				 MessageBox::Show("Calibration Finished, device added to Listbox check offset");
			 }


			 //används inte
	private: System::Void backgroundWorker1_RunWorkerCompleted(System::Object^  sender, System::ComponentModel::RunWorkerCompletedEventArgs^  e)
	{

	}

	private: System::Void btn_start_Signal_Lamp_Calibration_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if (myRobot->get_Robot_Status() == true)//robot socket is connected
		{
			if (lstCalibrationPreview->Items->Count > 0)//device list is not empty
			{
				cout << "started robot calibration" << endl;
				robotCalibrationThread->RunWorkerAsync();
			}
			else
			{
				MessageBox::Show("No Reference Device Added");
			}
		}
	}
	private: System::Void btn_goto_grid_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if (Signal_Data_Exist)
		{
			try
			{
				int Rows_limit;//10;//set from UI
				int Col_limit;// = 10;
				int step_size_pan;// = 50;//default
				int step_size_tilt;// = 10;

				get_Automation_Variables(&Rows_limit, &Col_limit, &step_size_pan, &step_size_tilt);

				std::string string_col_nr = msclr::interop::marshal_as<std::string>(txt_col_nr->Text);
				int Col_nr = std::stoi(string_col_nr);
				std::string string_row_nr = msclr::interop::marshal_as<std::string>(txt_row_nr->Text);
				int Row_nr = std::stoi(string_row_nr);

				myVideoStreamerObject->Set_Grid_Reference_Axis(Col_nr, Row_nr);

				for (int j = 0; j < Rows_limit; j++)
				{
					for (int i = 0; i < Col_limit; i++)
					{
						if (signal_lamp_data[j][i].col == Col_nr && signal_lamp_data[j][i].row == Row_nr)
						{
							myRobot->set_pos(signal_lamp_data[j][i].pan, signal_lamp_data[j][i].tilt);
						}
					}
				}
			}
			catch (...)
			{

			}
		}
		else
		{
			MessageBox::Show("No signal lamp data exist!");
		}
	}
	};
}
