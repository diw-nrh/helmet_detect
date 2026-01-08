// MyForm.h
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
	// โค้ดของปุ่ม Snapshot
	inline System::Void CaptureForm::btnSnapshot_Click(System::Object^ sender, System::EventArgs^ e) {
		if (linkedCameraForm == nullptr) return;
		Bitmap^ snapshot = GetSnapshotFromCamera();
		if (snapshot != nullptr) {
			if (pictureBox1->Image != nullptr) delete pictureBox1->Image;
			pictureBox1->Image = snapshot;

			// ตอนนี้จะเรียกฟังก์ชันนี้ได้แล้ว เพราะอยู่หลัง #include "cameraForm.h"
			linkedCameraForm->GetLatestDetectionData(*lastBoxes, *lastConfidences, *lastCapturedMat);
		}
	}

	// โค้ดของ Double Click ใน MyForm.h
	inline System::Void CaptureForm::pictureBox1_MouseDoubleClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
		if (lastBoxes->empty() || pictureBox1->Image == nullptr || lastCapturedMat->empty()) return;

		float ratioX = (float)pictureBox1->Image->Width / pictureBox1->Width;
		float ratioY = (float)pictureBox1->Image->Height / pictureBox1->Height;

		int clickX = (int)(e->X * ratioX);
		int clickY = (int)(e->Y * ratioY);

		for (size_t i = 0; i < lastBoxes->size(); i++) {
			if ((*lastBoxes)[i].contains(cv::Point(clickX, clickY))) {

				// 🔥 จุดที่ต้องแก้: สร้าง Safe ROI เพื่อไม่ให้พิกัดล้นขอบภาพ
				cv::Rect imgBounds(0, 0, lastCapturedMat->cols, lastCapturedMat->rows);
				cv::Rect safeBox = (*lastBoxes)[i] & imgBounds; // ใช้ตัวแปร & หาพื้นที่ทับซ้อนที่อยู่ในภาพเท่านั้น

				// ตรวจสอบว่าหลังจากตัดขอบที่ล้นออกแล้ว กรอบยังมีขนาดอยู่จริง
				if (safeBox.width <= 0 || safeBox.height <= 0) continue;

				float conf = (*lastConfidences)[i] * 100;

				// ใช้ safeBox ในการสร้าง ROI แทนอันเดิม
				cv::Mat roi = (*lastCapturedMat)(safeBox);

				// ส่วนที่เหลือเหมือนเดิม
				System::Diagnostics::Debug::WriteLine("ROI Size: " + roi.cols + "x" + roi.rows);
				cv::Scalar avg = cv::mean(roi);
				System::Diagnostics::Debug::WriteLine("Average BGR: B=" + avg[0] + " G=" + avg[1] + " R=" + avg[2]);
				std::string color = GetHelmetColor(roi);

				MessageBox::Show("Helmet #" + (i + 1) +
					"\nConfidence: " + conf.ToString("F2") + "%" +
					"\nDetected Color: " + gcnew String(color.c_str()),
					"Helmet Info");
				return;
			}
		}
	}
}

namespace MyFormApp {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
		}

	private:
		helmetdetect::cameraForm^ activeCamForm = nullptr;

	protected:
		~MyForm()
		{
			if (components) delete components;
		}

	private: 
		System::Windows::Forms::Panel^ panelMain;
		System::Windows::Forms::Panel^ panelHeader;
		System::Windows::Forms::Panel^ panelAI;
		System::Windows::Forms::Panel^ panelNightVision;
		System::Windows::Forms::Panel^ panelBrightness;
		System::Windows::Forms::Button^ btnOpenCamera;
		System::Windows::Forms::TrackBar^ nightTrack;
		System::Windows::Forms::TrackBar^ aiTrack;
		System::Windows::Forms::Label^ labelNightBrightness;
		System::Windows::Forms::Label^ labelAIConfidence;
		System::Windows::Forms::Label^ labelTitle;
		System::Windows::Forms::Label^ labelAITitle;
		System::Windows::Forms::Label^ labelNightTitle;
		System::Windows::Forms::Label^ labelBrightnessTitle;
		System::Windows::Forms::Button^ blacknightvision;
		System::Windows::Forms::Button^ greennightvision;
		System::ComponentModel::IContainer^ components;
		bool isNight = false;
		bool isNightGreen = false;
		double currentClipLimit = 4.0;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->panelMain = (gcnew System::Windows::Forms::Panel());
			this->panelBrightness = (gcnew System::Windows::Forms::Panel());
			this->labelBrightnessTitle = (gcnew System::Windows::Forms::Label());
			this->nightTrack = (gcnew System::Windows::Forms::TrackBar());
			this->labelNightBrightness = (gcnew System::Windows::Forms::Label());
			this->panelNightVision = (gcnew System::Windows::Forms::Panel());
			this->labelNightTitle = (gcnew System::Windows::Forms::Label());
			this->greennightvision = (gcnew System::Windows::Forms::Button());
			this->blacknightvision = (gcnew System::Windows::Forms::Button());
			this->panelAI = (gcnew System::Windows::Forms::Panel());
			this->labelAITitle = (gcnew System::Windows::Forms::Label());
			this->aiTrack = (gcnew System::Windows::Forms::TrackBar());
			this->labelAIConfidence = (gcnew System::Windows::Forms::Label());
			this->panelHeader = (gcnew System::Windows::Forms::Panel());
			this->labelTitle = (gcnew System::Windows::Forms::Label());
			this->btnOpenCamera = (gcnew System::Windows::Forms::Button());
			this->panelMain->SuspendLayout();
			this->panelBrightness->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->nightTrack))->BeginInit();
			this->panelNightVision->SuspendLayout();
			this->panelAI->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->aiTrack))->BeginInit();
			this->panelHeader->SuspendLayout();
			this->SuspendLayout();
			// 
			// panelMain
			// 
			this->panelMain->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(45)), static_cast<System::Int32>(static_cast<System::Byte>(45)),
				static_cast<System::Int32>(static_cast<System::Byte>(48)));
			this->panelMain->Controls->Add(this->panelBrightness);
			this->panelMain->Controls->Add(this->panelNightVision);
			this->panelMain->Controls->Add(this->panelAI);
			this->panelMain->Controls->Add(this->panelHeader);
			this->panelMain->Dock = System::Windows::Forms::DockStyle::Fill;
			this->panelMain->Location = System::Drawing::Point(0, 0);
			this->panelMain->Name = L"panelMain";
			this->panelMain->Padding = System::Windows::Forms::Padding(10);
			this->panelMain->Size = System::Drawing::Size(420, 520);
			this->panelMain->TabIndex = 0;
			// 
			// panelBrightness
			// 
			this->panelBrightness->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(60)), static_cast<System::Int32>(static_cast<System::Byte>(60)),
				static_cast<System::Int32>(static_cast<System::Byte>(65)));
			this->panelBrightness->Controls->Add(this->labelBrightnessTitle);
			this->panelBrightness->Controls->Add(this->nightTrack);
			this->panelBrightness->Controls->Add(this->labelNightBrightness);
			this->panelBrightness->Dock = System::Windows::Forms::DockStyle::Top;
			this->panelBrightness->Location = System::Drawing::Point(10, 360);
			this->panelBrightness->Name = L"panelBrightness";
			this->panelBrightness->Padding = System::Windows::Forms::Padding(15);
			this->panelBrightness->Size = System::Drawing::Size(400, 120);
			this->panelBrightness->TabIndex = 3;
			// 
			// labelBrightnessTitle
			// 
			this->labelBrightnessTitle->AutoSize = true;
			this->labelBrightnessTitle->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10, System::Drawing::FontStyle::Bold));
			this->labelBrightnessTitle->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(122)), static_cast<System::Int32>(static_cast<System::Byte>(204)));
			this->labelBrightnessTitle->Location = System::Drawing::Point(15, 15);
			this->labelBrightnessTitle->Name = L"labelBrightnessTitle";
			this->labelBrightnessTitle->Size = System::Drawing::Size(174, 23);
			this->labelBrightnessTitle->TabIndex = 0;
			this->labelBrightnessTitle->Text = L"🌙 Night Brightness";
			// 
			// nightTrack
			// 
			this->nightTrack->Location = System::Drawing::Point(15, 65);
			this->nightTrack->Minimum = 1;
			this->nightTrack->Name = L"nightTrack";
			this->nightTrack->Size = System::Drawing::Size(370, 56);
			this->nightTrack->TabIndex = 1;
			this->nightTrack->TickStyle = System::Windows::Forms::TickStyle::Both;
			this->nightTrack->Value = 4;
			this->nightTrack->Scroll += gcnew System::EventHandler(this, &MyForm::trackBar1_Scroll_1);
			// 
			// labelNightBrightness
			// 
			this->labelNightBrightness->AutoSize = true;
			this->labelNightBrightness->Font = (gcnew System::Drawing::Font(L"Segoe UI", 9));
			this->labelNightBrightness->ForeColor = System::Drawing::Color::White;
			this->labelNightBrightness->Location = System::Drawing::Point(15, 40);
			this->labelNightBrightness->Name = L"labelNightBrightness";
			this->labelNightBrightness->Size = System::Drawing::Size(58, 20);
			this->labelNightBrightness->TabIndex = 2;
			this->labelNightBrightness->Text = L"Level: 4";
			// 
			// panelNightVision
			// 
			this->panelNightVision->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(60)), static_cast<System::Int32>(static_cast<System::Byte>(60)),
				static_cast<System::Int32>(static_cast<System::Byte>(65)));
			this->panelNightVision->Controls->Add(this->labelNightTitle);
			this->panelNightVision->Controls->Add(this->greennightvision);
			this->panelNightVision->Controls->Add(this->blacknightvision);
			this->panelNightVision->Dock = System::Windows::Forms::DockStyle::Top;
			this->panelNightVision->Location = System::Drawing::Point(10, 240);
			this->panelNightVision->Name = L"panelNightVision";
			this->panelNightVision->Padding = System::Windows::Forms::Padding(15);
			this->panelNightVision->Size = System::Drawing::Size(400, 120);
			this->panelNightVision->TabIndex = 2;
			// 
			// labelNightTitle
			// 
			this->labelNightTitle->AutoSize = true;
			this->labelNightTitle->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10, System::Drawing::FontStyle::Bold));
			this->labelNightTitle->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(122)),
				static_cast<System::Int32>(static_cast<System::Byte>(204)));
			this->labelNightTitle->Location = System::Drawing::Point(15, 15);
			this->labelNightTitle->Name = L"labelNightTitle";
			this->labelNightTitle->Size = System::Drawing::Size(189, 23);
			this->labelNightTitle->TabIndex = 0;
			this->labelNightTitle->Text = L"👁 Night Vision Mode";
			// 
			// greennightvision
			// 
			this->greennightvision->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(120)),
				static_cast<System::Int32>(static_cast<System::Byte>(70)));
			this->greennightvision->FlatAppearance->BorderSize = 0;
			this->greennightvision->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->greennightvision->Font = (gcnew System::Drawing::Font(L"Segoe UI", 9, System::Drawing::FontStyle::Bold));
			this->greennightvision->ForeColor = System::Drawing::Color::White;
			this->greennightvision->Location = System::Drawing::Point(210, 55);
			this->greennightvision->Name = L"greennightvision";
			this->greennightvision->Size = System::Drawing::Size(175, 45);
			this->greennightvision->TabIndex = 2;
			this->greennightvision->Text = L"🟢 Green Night";
			this->greennightvision->UseVisualStyleBackColor = false;
			this->greennightvision->Click += gcnew System::EventHandler(this, &MyForm::greennightvision_Click);
			// 
			// blacknightvision
			// 
			this->blacknightvision->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(70)), static_cast<System::Int32>(static_cast<System::Byte>(70)),
				static_cast<System::Int32>(static_cast<System::Byte>(75)));
			this->blacknightvision->FlatAppearance->BorderSize = 0;
			this->blacknightvision->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->blacknightvision->Font = (gcnew System::Drawing::Font(L"Segoe UI", 9, System::Drawing::FontStyle::Bold));
			this->blacknightvision->ForeColor = System::Drawing::Color::White;
			this->blacknightvision->Location = System::Drawing::Point(15, 55);
			this->blacknightvision->Name = L"blacknightvision";
			this->blacknightvision->Size = System::Drawing::Size(175, 45);
			this->blacknightvision->TabIndex = 1;
			this->blacknightvision->Text = L"⚫ Black Night";
			this->blacknightvision->UseVisualStyleBackColor = false;
			this->blacknightvision->Click += gcnew System::EventHandler(this, &MyForm::blacknightvision_Click);
			// 
			// panelAI
			// 
			this->panelAI->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(60)), static_cast<System::Int32>(static_cast<System::Byte>(60)),
				static_cast<System::Int32>(static_cast<System::Byte>(65)));
			this->panelAI->Controls->Add(this->labelAITitle);
			this->panelAI->Controls->Add(this->aiTrack);
			this->panelAI->Controls->Add(this->labelAIConfidence);
			this->panelAI->Dock = System::Windows::Forms::DockStyle::Top;
			this->panelAI->Location = System::Drawing::Point(10, 120);
			this->panelAI->Name = L"panelAI";
			this->panelAI->Padding = System::Windows::Forms::Padding(15);
			this->panelAI->Size = System::Drawing::Size(400, 120);
			this->panelAI->TabIndex = 1;
			// 
			// labelAITitle
			// 
			this->labelAITitle->AutoSize = true;
			this->labelAITitle->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10, System::Drawing::FontStyle::Bold));
			this->labelAITitle->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(122)),
				static_cast<System::Int32>(static_cast<System::Byte>(204)));
			this->labelAITitle->Location = System::Drawing::Point(15, 15);
			this->labelAITitle->Name = L"labelAITitle";
			this->labelAITitle->Size = System::Drawing::Size(151, 23);
			this->labelAITitle->TabIndex = 0;
			this->labelAITitle->Text = L"🤖 AI Confidence";
			// 
			// aiTrack
			// 
			this->aiTrack->Location = System::Drawing::Point(15, 65);
			this->aiTrack->Maximum = 100;
			this->aiTrack->Minimum = 1;
			this->aiTrack->Name = L"aiTrack";
			this->aiTrack->Size = System::Drawing::Size(370, 56);
			this->aiTrack->TabIndex = 1;
			this->aiTrack->TickFrequency = 10;
			this->aiTrack->TickStyle = System::Windows::Forms::TickStyle::Both;
			this->aiTrack->Value = 50;
			this->aiTrack->Scroll += gcnew System::EventHandler(this, &MyForm::aiTrack_Scroll);
			// 
			// labelAIConfidence
			// 
			this->labelAIConfidence->AutoSize = true;
			this->labelAIConfidence->Font = (gcnew System::Drawing::Font(L"Segoe UI", 9));
			this->labelAIConfidence->ForeColor = System::Drawing::Color::White;
			this->labelAIConfidence->Location = System::Drawing::Point(15, 40);
			this->labelAIConfidence->Name = L"labelAIConfidence";
			this->labelAIConfidence->Size = System::Drawing::Size(109, 20);
			this->labelAIConfidence->TabIndex = 2;
			this->labelAIConfidence->Text = L"Threshold: 50%";
			// 
			// panelHeader
			// 
			this->panelHeader->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(60)), static_cast<System::Int32>(static_cast<System::Byte>(60)),
				static_cast<System::Int32>(static_cast<System::Byte>(65)));
			this->panelHeader->Controls->Add(this->labelTitle);
			this->panelHeader->Controls->Add(this->btnOpenCamera);
			this->panelHeader->Dock = System::Windows::Forms::DockStyle::Top;
			this->panelHeader->Location = System::Drawing::Point(10, 10);
			this->panelHeader->Name = L"panelHeader";
			this->panelHeader->Padding = System::Windows::Forms::Padding(15);
			this->panelHeader->Size = System::Drawing::Size(400, 110);
			this->panelHeader->TabIndex = 0;
			// 
			// labelTitle
			// 
			this->labelTitle->AutoSize = true;
			this->labelTitle->Font = (gcnew System::Drawing::Font(L"Segoe UI", 12, System::Drawing::FontStyle::Bold));
			this->labelTitle->ForeColor = System::Drawing::Color::White;
			this->labelTitle->Location = System::Drawing::Point(95, 15);
			this->labelTitle->Name = L"labelTitle";
			this->labelTitle->Size = System::Drawing::Size(214, 28);
			this->labelTitle->TabIndex = 0;
			this->labelTitle->Text = L"🛡 Helmet Detection";
			// 
			// btnOpenCamera
			// 
			this->btnOpenCamera->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(122)),
				static_cast<System::Int32>(static_cast<System::Byte>(204)));
			this->btnOpenCamera->FlatAppearance->BorderSize = 0;
			this->btnOpenCamera->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btnOpenCamera->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10, System::Drawing::FontStyle::Bold));
			this->btnOpenCamera->ForeColor = System::Drawing::Color::White;
			this->btnOpenCamera->Location = System::Drawing::Point(100, 50);
			this->btnOpenCamera->Name = L"btnOpenCamera";
			this->btnOpenCamera->Size = System::Drawing::Size(200, 45);
			this->btnOpenCamera->TabIndex = 1;
			this->btnOpenCamera->Text = L"📹 Open Camera";
			this->btnOpenCamera->UseVisualStyleBackColor = false;
			this->btnOpenCamera->Click += gcnew System::EventHandler(this, &MyForm::btnOpenCamera_Click);
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(420, 520);
			this->Controls->Add(this->panelMain);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->Name = L"MyForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"Helmet Detection Control";
			this->Load += gcnew System::EventHandler(this, &MyForm::MyForm_Load);
			this->panelMain->ResumeLayout(false);
			this->panelBrightness->ResumeLayout(false);
			this->panelBrightness->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->nightTrack))->EndInit();
			this->panelNightVision->ResumeLayout(false);
			this->panelNightVision->PerformLayout();
			this->panelAI->ResumeLayout(false);
			this->panelAI->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->aiTrack))->EndInit();
			this->panelHeader->ResumeLayout(false);
			this->panelHeader->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

	private: System::Void MyForm_Load(System::Object^ sender, System::EventArgs^ e) {
		labelNightBrightness->Text = "Level: " + nightTrack->Value.ToString();
		labelAIConfidence->Text = "Threshold: " + aiTrack->Value.ToString() + "%";
	}

	private: System::Void btnOpenCamera_Click(System::Object^ sender, System::EventArgs^ e) {
		try {
			activeCamForm = gcnew helmetdetect::cameraForm();
			helmetdetect::CaptureForm^ captureForm = gcnew helmetdetect::CaptureForm();
			captureForm->SetCameraForm(activeCamForm);
			
			activeCamForm->isNight = this->isNight;
			activeCamForm->isNightGreen = this->isNightGreen;
			activeCamForm->currentClipLimit = this->currentClipLimit;
			activeCamForm->aiConfidence = (float)aiTrack->Value / 100.0f;

			activeCamForm->Show();
			captureForm->Show();
		}
		catch (System::Exception^ ex) {
			MessageBox::Show("Error: " + ex->Message, "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
		}
	}

	private: System::Void blacknightvision_Click(System::Object^ sender, System::EventArgs^ e) {
		isNight = !isNight;
		if (isNight) {
			blacknightvision->BackColor = Color::FromArgb(100, 150, 255);
			blacknightvision->Text = L"⚫ Active";
			isNightGreen = false;
			greennightvision->BackColor = Color::FromArgb(0, 120, 70);
			greennightvision->Text = L"🟢 Green Night";
		}
		else {
			blacknightvision->BackColor = Color::FromArgb(70, 70, 75);
			blacknightvision->Text = L"⚫ Black Night";
		}

		if (activeCamForm != nullptr && !activeCamForm->IsDisposed) {
			activeCamForm->isNight = this->isNight;
			activeCamForm->isNightGreen = this->isNightGreen;
		}
	}

	private: System::Void greennightvision_Click(System::Object^ sender, System::EventArgs^ e) {
		isNightGreen = !isNightGreen;
		if (isNightGreen) {
			greennightvision->BackColor = Color::FromArgb(100, 255, 150);
			greennightvision->Text = L"🟢 Active";
			isNight = false;
			blacknightvision->BackColor = Color::FromArgb(70, 70, 75);
			blacknightvision->Text = L"⚫ Black Night";
		}
		else {
			greennightvision->BackColor = Color::FromArgb(0, 120, 70);
			greennightvision->Text = L"🟢 Green Night";
		}

		if (activeCamForm != nullptr && !activeCamForm->IsDisposed) {
			activeCamForm->isNight = this->isNight;
			activeCamForm->isNightGreen = this->isNightGreen;
		}
	}

	private: System::Void trackBar1_Scroll_1(System::Object^ sender, System::EventArgs^ e) {
		currentClipLimit = (double)nightTrack->Value;
		labelNightBrightness->Text = "Level: " + nightTrack->Value.ToString();

		if (activeCamForm != nullptr && !activeCamForm->IsDisposed) {
			activeCamForm->currentClipLimit = this->currentClipLimit;
		}
	}

	private: System::Void aiTrack_Scroll(System::Object^ sender, System::EventArgs^ e) {
		float threshold = (float)aiTrack->Value / 100.0f;
		labelAIConfidence->Text = "Threshold: " + aiTrack->Value.ToString() + "%";

		if (activeCamForm != nullptr && !activeCamForm->IsDisposed) {
			activeCamForm->aiConfidence = threshold;
		}
	}
	};
}
