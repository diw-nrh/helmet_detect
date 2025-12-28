#pragma once
#include "cameraForm.h"
#include "CaptureForm.h"

	// Implement CaptureForm method after cameraForm is fully defined
namespace helmetdetect {
	inline Bitmap^ CaptureForm::GetSnapshotFromCamera() {
		if (linkedCameraForm != nullptr) {
			return linkedCameraForm->GetCurrentFrame();
		}
		return nullptr;
	}
}

namespace MyFormApp {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^ btnOpenCamera;
	private: System::Windows::Forms::Timer^ timer1;
	private: System::Windows::Forms::TrackBar^ trackBar1;
	private: System::Windows::Forms::TrackBar^ trackBar2;
	private: System::Windows::Forms::Button^ button1;


	private: System::ComponentModel::IContainer^ components;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->btnOpenCamera = (gcnew System::Windows::Forms::Button());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->trackBar1 = (gcnew System::Windows::Forms::TrackBar());
			this->trackBar2 = (gcnew System::Windows::Forms::TrackBar());
			this->button1 = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar2))->BeginInit();
			this->SuspendLayout();
			// 
			// btnOpenCamera
			// 
			this->btnOpenCamera->Location = System::Drawing::Point(103, 47);
			this->btnOpenCamera->Name = L"btnOpenCamera";
			this->btnOpenCamera->Size = System::Drawing::Size(159, 35);
			this->btnOpenCamera->TabIndex = 0;
			this->btnOpenCamera->Text = L"Open Camera";
			this->btnOpenCamera->UseVisualStyleBackColor = true;
			this->btnOpenCamera->Click += gcnew System::EventHandler(this, &MyForm::btnOpenCamera_Click);
			// 
			// trackBar1
			// 
			this->trackBar1->Location = System::Drawing::Point(56, 88);
			this->trackBar1->Name = L"trackBar1";
			this->trackBar1->Size = System::Drawing::Size(259, 56);
			this->trackBar1->TabIndex = 1;
			// 
			// trackBar2
			// 
			this->trackBar2->Location = System::Drawing::Point(56, 150);
			this->trackBar2->Name = L"trackBar2";
			this->trackBar2->Size = System::Drawing::Size(259, 56);
			this->trackBar2->TabIndex = 2;
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(103, 212);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(159, 39);
			this->button1->TabIndex = 3;
			this->button1->Text = L"button1";
			this->button1->UseVisualStyleBackColor = true;
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(368, 349);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->trackBar2);
			this->Controls->Add(this->trackBar1);
			this->Controls->Add(this->btnOpenCamera);
			this->Name = L"MyForm";
			this->Text = L"MyForm";
			this->Load += gcnew System::EventHandler(this, &MyForm::MyForm_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar2))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void MyForm_Load(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void btnOpenCamera_Click(System::Object^ sender, System::EventArgs^ e) {
		try {
			// Create both forms
			helmetdetect::cameraForm^ cameraForm = gcnew helmetdetect::cameraForm();
			helmetdetect::CaptureForm^ captureForm = gcnew helmetdetect::CaptureForm();
			
			// Link capture form with camera form
			captureForm->SetCameraForm(cameraForm);
			
			// Show both forms (non-modal so they can be used simultaneously)
			cameraForm->Show();
			captureForm->Show();
		}
		catch (System::AccessViolationException^ ex) {
			MessageBox::Show("Camera initialization failed: " + ex->Message, 
				"Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
		}
		catch (System::Exception^ ex) {
			MessageBox::Show("Error opening camera form: " + ex->Message, 
				"Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
		}
	}
	private: System::Void trackBar1_Scroll(System::Object^ sender, System::EventArgs^ e) {
	}
private: System::Void trackBar2_Scroll(System::Object^ sender, System::EventArgs^ e) {
}
};
}
