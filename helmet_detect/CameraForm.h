#pragma once
#include <opencv2/opencv.hpp>
#include <msclr/marshal_cppstd.h>

namespace helmetdetect {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Drawing::Imaging;

	public ref class cameraForm : public System::Windows::Forms::Form
	{
	public:
		cameraForm(void)
		{
			InitializeComponent();
			capture = nullptr;
			isStart = false;
			currentFrame = nullptr;
		}

		// Public method to get current frame
		Bitmap^ GetCurrentFrame() {
			if (currentFrame != nullptr) {
				return safe_cast<Bitmap^>(currentFrame->Clone());
			}
			return nullptr;
		}

	protected:
		~cameraForm()
		{
			if (components)
			{
				delete components;
			}
			// Clean up camera
			if (capture != nullptr && capture->isOpened())
			{
				capture->release();
				delete capture;
			}
			if (currentFrame != nullptr) {
				delete currentFrame;
			}
		}

	private: System::Windows::Forms::PictureBox^ pictureBox1;
	private: System::Windows::Forms::Button^ StartButton;
	private: System::Windows::Forms::Timer^ timer1;
	private: System::ComponentModel::IContainer^ components;

	private:
		cv::VideoCapture* capture;
		bool isStart;
		Bitmap^ currentFrame;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->StartButton = (gcnew System::Windows::Forms::Button());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			this->SuspendLayout();
			// 
			// pictureBox1
			// 
			this->pictureBox1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->pictureBox1->Location = System::Drawing::Point(0, 0);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(491, 443);
			this->pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBox1->TabIndex = 0;
			this->pictureBox1->TabStop = false;
			// 
			// StartButton
			// 
			this->StartButton->Location = System::Drawing::Point(380, 379);
			this->StartButton->Name = L"StartButton";
			this->StartButton->Size = System::Drawing::Size(75, 30);
			this->StartButton->TabIndex = 2;
			this->StartButton->Text = L"Start";
			this->StartButton->UseVisualStyleBackColor = true;
			this->StartButton->Click += gcnew System::EventHandler(this, &cameraForm::StartButton_Click);
			// 
			// timer1
			// 
			this->timer1->Interval = 33;
			this->timer1->Tick += gcnew System::EventHandler(this, &cameraForm::timer1_Tick);
			// 
			// cameraForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(491, 443);
			this->Controls->Add(this->StartButton);
			this->Controls->Add(this->pictureBox1);
			this->Name = L"cameraForm";
			this->Text = L"Camera View";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &cameraForm::cameraForm_FormClosing);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

	private: System::Void StartButton_Click(System::Object^ sender, System::EventArgs^ e) {
		if (isStart) {
			// Stop camera
			isStart = false;
			timer1->Stop();
			StartButton->Text = L"Start";

			if (capture != nullptr && capture->isOpened()) {
				capture->release();
			}
			return;
		}

		// Start camera
		if (capture == nullptr) {
			capture = new cv::VideoCapture(0);
		}
		else if (!capture->isOpened()) {
			capture->open(0);
		}

		if (!capture->isOpened()) {
			MessageBox::Show("Failed to open camera!", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
			return;
		}

		// Get camera properties
		int capWidth = (int)capture->get(cv::CAP_PROP_FRAME_WIDTH);
		int capHeight = (int)capture->get(cv::CAP_PROP_FRAME_HEIGHT);
		int capFPS = (int)capture->get(cv::CAP_PROP_FPS);

		// Start timer to update frames
		isStart = true;
		timer1->Start();
		StartButton->Text = L"Stop";
	}

	private: System::Void timer1_Tick(System::Object^ sender, System::EventArgs^ e) {
		if (capture != nullptr && capture->isOpened()) {
			cv::Mat frame;
			*capture >> frame;

			if (!frame.empty()) {
				// Convert Mat to Bitmap
				Bitmap^ bmp = MatToBitmap(frame);

				// Store current frame
				if (currentFrame != nullptr) {
					delete currentFrame;
				}
				currentFrame = safe_cast<Bitmap^>(bmp->Clone());

				// Display in PictureBox
				if (pictureBox1->Image != nullptr) {
					delete pictureBox1->Image;
				}
				pictureBox1->Image = bmp;
			}
		}
	}

	private: System::Void cameraForm_FormClosing(System::Object^ sender, FormClosingEventArgs^ e) {
		if (timer1->Enabled) {
			timer1->Stop();
		}
		if (capture != nullptr && capture->isOpened()) {
			capture->release();
		}
	}

	// Convert cv::Mat to System::Drawing::Bitmap
	private: Bitmap^ MatToBitmap(cv::Mat& mat) {
		// No color conversion needed - Format24bppRgb actually stores BGR in memory
		cv::Mat temp;
		if (!mat.isContinuous()) {
			temp = mat.clone();
		} else {
			temp = mat;
		}

		// Create Bitmap with proper memory management
		Bitmap^ bitmap = gcnew Bitmap(temp.cols, temp.rows, PixelFormat::Format24bppRgb);

		// Lock bitmap data for writing
		System::Drawing::Rectangle rect(0, 0, temp.cols, temp.rows);
		BitmapData^ bmpData = bitmap->LockBits(rect, ImageLockMode::WriteOnly, PixelFormat::Format24bppRgb);

		// Copy pixel data from Mat to Bitmap
		unsigned char* ptrSrc = temp.data;
		unsigned char* ptrDst = (unsigned char*)bmpData->Scan0.ToPointer();
		int srcStride = static_cast<int>(temp.step);
		int dstStride = bmpData->Stride;
		int rowSize = temp.cols * 3; // 3 bytes per pixel (BGR)

		for (int y = 0; y < temp.rows; y++) {
			memcpy(ptrDst + y * dstStride, ptrSrc + y * srcStride, rowSize);
		}

		// Unlock bitmap
		bitmap->UnlockBits(bmpData);

		return bitmap;
	}
	};
}
