#pragma once

using namespace System::Windows::Forms;

namespace CalibrationApp {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for Add_Position_Dialog_Box
	/// </summary>
	public ref class Add_Position_Dialog_Box : public System::Windows::Forms::Form
	{
	public:
		Form ^parentForm;

		Add_Position_Dialog_Box(Form ^frm)
		{
			parentForm = frm;
			InitializeComponent();
			device_type = "";
			//
			//TODO: Add the constructor code here
			//
		}
		System::String^ getDeviceType()
		{
			return device_type;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Add_Position_Dialog_Box()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  btn_Add_Signal_res;
	private: System::Windows::Forms::Button^  btn_Add_Zoom_res;
	protected:


	private: System::Windows::Forms::Button^  btn_Add_IR_res;


	protected:

	protected:

	protected:

	private:
		System::String ^device_type;
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->btn_Add_Signal_res = (gcnew System::Windows::Forms::Button());
			this->btn_Add_Zoom_res = (gcnew System::Windows::Forms::Button());
			this->btn_Add_IR_res = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// btn_Add_Signal_res
			// 
			this->btn_Add_Signal_res->Location = System::Drawing::Point(68, 174);
			this->btn_Add_Signal_res->Name = L"btn_Add_Signal_res";
			this->btn_Add_Signal_res->Size = System::Drawing::Size(135, 26);
			this->btn_Add_Signal_res->TabIndex = 0;
			this->btn_Add_Signal_res->Text = L"Signal Lamp";
			this->btn_Add_Signal_res->UseVisualStyleBackColor = true;
			this->btn_Add_Signal_res->Click += gcnew System::EventHandler(this, &Add_Position_Dialog_Box::Dialog_Button_Click);
			// 
			// btn_Add_Zoom_res
			// 
			this->btn_Add_Zoom_res->Location = System::Drawing::Point(68, 110);
			this->btn_Add_Zoom_res->Name = L"btn_Add_Zoom_res";
			this->btn_Add_Zoom_res->Size = System::Drawing::Size(135, 26);
			this->btn_Add_Zoom_res->TabIndex = 1;
			this->btn_Add_Zoom_res->Text = L"Zoom_Cam";
			this->btn_Add_Zoom_res->UseVisualStyleBackColor = true;
			this->btn_Add_Zoom_res->Click += gcnew System::EventHandler(this, &Add_Position_Dialog_Box::Dialog_Button_Click);
			// 
			// btn_Add_IR_res
			// 
			this->btn_Add_IR_res->Location = System::Drawing::Point(68, 142);
			this->btn_Add_IR_res->Name = L"btn_Add_IR_res";
			this->btn_Add_IR_res->Size = System::Drawing::Size(135, 26);
			this->btn_Add_IR_res->TabIndex = 2;
			this->btn_Add_IR_res->Text = L"IR_Cam";
			this->btn_Add_IR_res->UseVisualStyleBackColor = true;
			this->btn_Add_IR_res->Click += gcnew System::EventHandler(this, &Add_Position_Dialog_Box::Dialog_Button_Click);
			// 
			// Add_Position_Dialog_Box
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(282, 255);
			this->Controls->Add(this->btn_Add_IR_res);
			this->Controls->Add(this->btn_Add_Zoom_res);
			this->Controls->Add(this->btn_Add_Signal_res);
			this->Name = L"Add_Position_Dialog_Box";
			this->Text = L"Add_Position_Dialog_Box";
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void Dialog_Button_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		if (sender == btn_Add_Zoom_res)
		{
			device_type = "zoom";
		}
		else if (sender == btn_Add_IR_res)
		{
			device_type = "ir";
		}
		else if (sender == btn_Add_Signal_res)
		{
			device_type = "signal";
		}
		DialogResult = ::DialogResult::OK;
	}
	};
}
