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

	private:
		// เก็บตัวแปรไว้ที่นี่เพื่อให้เข้าถึงได้จากทุก Event ใน MyForm
		helmetdetect::cameraForm^ activeCamForm = nullptr;

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
	private: System::Windows::Forms::TrackBar^ nightTrack;
	private: System::Windows::Forms::TrackBar^ aiTrack;



	private: System::Windows::Forms::Button^ blacknightvision;
	private: System::Windows::Forms::Button^ greennightvision;
	private: bool isNight = false;
	private: bool isNightGreen = false;



	private: System::ComponentModel::IContainer^ components;

#pragma region Windows Form Designer generated code
		   void InitializeComponent(void)
		   {
			   this->components = (gcnew System::ComponentModel::Container());
			   this->btnOpenCamera = (gcnew System::Windows::Forms::Button());
			   this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			   this->nightTrack = (gcnew System::Windows::Forms::TrackBar());
			   this->aiTrack = (gcnew System::Windows::Forms::TrackBar());
			   this->blacknightvision = (gcnew System::Windows::Forms::Button());
			   this->greennightvision = (gcnew System::Windows::Forms::Button());
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->nightTrack))->BeginInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->aiTrack))->BeginInit();
			   this->SuspendLayout();
			   // 
			   // btnOpenCamera
			   // 
			   this->btnOpenCamera->Location = System::Drawing::Point(103, 42);
			   this->btnOpenCamera->Name = L"btnOpenCamera";
			   this->btnOpenCamera->Size = System::Drawing::Size(159, 35);
			   this->btnOpenCamera->TabIndex = 0;
			   this->btnOpenCamera->Text = L"Open Camera";
			   this->btnOpenCamera->UseVisualStyleBackColor = true;
			   this->btnOpenCamera->Click += gcnew System::EventHandler(this, &MyForm::btnOpenCamera_Click);
			   // 
			   // nightTrack
			   // 
			   this->nightTrack->Location = System::Drawing::Point(56, 271);
			   this->nightTrack->Minimum = 1;
			   this->nightTrack->Name = L"nightTrack";
			   this->nightTrack->Size = System::Drawing::Size(259, 56);
			   this->nightTrack->TabIndex = 1;
			   this->nightTrack->Value = 1;
			   this->nightTrack->Scroll += gcnew System::EventHandler(this, &MyForm::trackBar1_Scroll_1);
			   // 
			   // aiTrack
			   // 
			   this->aiTrack->Location = System::Drawing::Point(56, 83);
			   this->aiTrack->Maximum = 100;
			   this->aiTrack->Minimum = 1;
			   this->aiTrack->Name = L"aiTrack";
			   this->aiTrack->Size = System::Drawing::Size(259, 56);
			   this->aiTrack->TabIndex = 2;
			   this->aiTrack->Value = 1;
			   this->aiTrack->Scroll += gcnew System::EventHandler(this, &MyForm::aiTrack_Scroll);
			   // 
			   // blacknightvision
			   // 
			   this->blacknightvision->Location = System::Drawing::Point(103, 181);
			   this->blacknightvision->Name = L"blacknightvision";
			   this->blacknightvision->Size = System::Drawing::Size(159, 39);
			   this->blacknightvision->TabIndex = 3;
			   this->blacknightvision->Text = L"B_Night";
			   this->blacknightvision->UseVisualStyleBackColor = true;
			   this->blacknightvision->Click += gcnew System::EventHandler(this, &MyForm::blacknightvision_Click);
			   // 
			   // greennightvision
			   // 
			   this->greennightvision->Location = System::Drawing::Point(103, 226);
			   this->greennightvision->Name = L"greennightvision";
			   this->greennightvision->Size = System::Drawing::Size(159, 39);
			   this->greennightvision->TabIndex = 4;
			   this->greennightvision->Text = L"G_Night";
			   this->greennightvision->UseVisualStyleBackColor = true;
			   this->greennightvision->Click += gcnew System::EventHandler(this, &MyForm::greennightvision_Click);
			   // 
			   // MyForm
			   // 
			   this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			   this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			   this->ClientSize = System::Drawing::Size(368, 349);
			   this->Controls->Add(this->greennightvision);
			   this->Controls->Add(this->blacknightvision);
			   this->Controls->Add(this->aiTrack);
			   this->Controls->Add(this->nightTrack);
			   this->Controls->Add(this->btnOpenCamera);
			   this->Name = L"MyForm";
			   this->Text = L"MyForm";
			   this->Load += gcnew System::EventHandler(this, &MyForm::MyForm_Load);
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->nightTrack))->EndInit();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->aiTrack))->EndInit();
			   this->ResumeLayout(false);
			   this->PerformLayout();

		   }

		   double currentClipLimit = 4.0; // ค่าเริ่มต้น
#pragma endregion
	private: System::Void MyForm_Load(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void btnOpenCamera_Click(System::Object^ sender, System::EventArgs^ e) {
		try {
			// ตรวจสอบถ้าเปิดอยู่แล้วให้ปิดก่อน หรือจัดการตามต้องการ
			activeCamForm = gcnew helmetdetect::cameraForm();
			helmetdetect::CaptureForm^ captureForm = gcnew helmetdetect::CaptureForm();

			captureForm->SetCameraForm(activeCamForm);

			// ส่งค่าเริ่มต้นจาก Trackbar/ปุ่ม ใน MyForm ไปยัง cameraForm ทันทีที่เปิด
			activeCamForm->isNight = this->isNight;
			activeCamForm->isNightGreen = this->isNightGreen;
			activeCamForm->currentClipLimit = this->currentClipLimit;

			activeCamForm->Show();
			captureForm->Show();
		}
		catch (System::Exception^ ex) {
			MessageBox::Show("Error: " + ex->Message);
		}
	}
	private: System::Void trackBar1_Scroll(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void trackBar2_Scroll(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void blacknightvision_Click(System::Object^ sender, System::EventArgs^ e) {
		isNight = !isNight;
		if (isNight) {
			blacknightvision->Text = "Color";
			isNightGreen = false;
			greennightvision->Text = "NightGreen"; // Reset อีกปุ่ม
		}
		else {
			blacknightvision->Text = "Night";
		}

		// 🔥 ส่งค่าไปยัง cameraForm ที่เปิดอยู่
		if (activeCamForm != nullptr && !activeCamForm->IsDisposed) {
			activeCamForm->isNight = this->isNight;
			activeCamForm->isNightGreen = this->isNightGreen;
		}

	}
	private: System::Void greennightvision_Click(System::Object^ sender, System::EventArgs^ e) {
		isNightGreen = !isNightGreen;
		if (isNightGreen) {
			greennightvision->Text = "Color";
			isNight = false;
			blacknightvision->Text = "Night"; // Reset อีกปุ่ม
		}
		else {
			greennightvision->Text = "NightGreen";
		}

		// 🔥 ส่งค่าไปยัง cameraForm ที่เปิดอยู่
		if (activeCamForm != nullptr && !activeCamForm->IsDisposed) {
			activeCamForm->isNight = this->isNight;
			activeCamForm->isNightGreen = this->isNightGreen;
		}
	}
	private: System::Void trackBar1_Scroll_1(System::Object^ sender, System::EventArgs^ e) {
		// เราต้องแปลงเป็น double เพราะ createCLAHE รับค่า double
		currentClipLimit = (double)nightTrack->Value;

		// (Optional) ถ้าอยากให้โชว์ตัวเลขว่าปรับไปเท่าไหร่แล้ว ให้ใส่ Label เพิ่ม
		// labelBrightness->Text = "Brightness: " + currentClipLimit.ToString();

		// 🔥 ส่งค่าไปยัง cameraForm ที่เปิดอยู่
		if (activeCamForm != nullptr && !activeCamForm->IsDisposed) {
			activeCamForm->currentClipLimit = this->currentClipLimit;
		}
	}
	private: System::Void aiTrack_Scroll(System::Object^ sender, System::EventArgs^ e) {
		float threshold = (float)aiTrack->Value / 100.0f; // แปลง 1-100 เป็น 0.01 - 1.0

		// ส่งค่าไปที่หน้ากล้อง
		if (activeCamForm != nullptr && !activeCamForm->IsDisposed) {
			activeCamForm->aiConfidence = threshold;
		}
	}
	};
}
