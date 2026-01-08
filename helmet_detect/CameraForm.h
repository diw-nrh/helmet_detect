#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <msclr/marshal_cppstd.h>
#include <vector>
#include <string>

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
		bool isNight;
		bool isNightGreen;
		double currentClipLimit;
		float aiConfidence;

	private:
		std::vector<cv::Rect>* currentBoxes;
		std::vector<float>* currentConfidences;
		cv::Mat* currentMatForCapture;
		cv::VideoCapture* capture;
		bool isStart;
		Bitmap^ currentFrame;
		cv::dnn::Net* net;
		
		System::Windows::Forms::Panel^ panelMain;
		System::Windows::Forms::Panel^ panelVideo;
		System::Windows::Forms::Panel^ panelControls;
		System::Windows::Forms::PictureBox^ pictureBox1;
		System::Windows::Forms::Button^ StartButton;
		System::Windows::Forms::Label^ labelStatus;
		System::Windows::Forms::Timer^ timer1;
		System::ComponentModel::IContainer^ components;

		const float INPUT_WIDTH = 640.0f;
		const float INPUT_HEIGHT = 640.0f;
		const float SCORE_THRESHOLD = 0.5f;
		const float NMS_THRESHOLD = 0.45f;

		int frameCount = 0;
		System::DateTime lastFPSUpdate;

	public:
		cameraForm(void)
		{
			InitializeComponent();
			capture = nullptr;
			isStart = false;
			currentFrame = nullptr;
			isNight = false;
			isNightGreen = false;
			currentClipLimit = 4.0;
			aiConfidence = 0.5f;
			currentBoxes = new std::vector<cv::Rect>();
			currentConfidences = new std::vector<float>();
			currentMatForCapture = new cv::Mat();
			net = nullptr;
			InitializeAI();
			lastFPSUpdate = System::DateTime::Now;
		}

		void GetLatestDetectionData(std::vector<cv::Rect>& outBoxes, std::vector<float>& outConfs, cv::Mat& outMat) {
			if (currentBoxes) outBoxes = *currentBoxes;
			if (currentConfidences) outConfs = *currentConfidences;
			if (currentMatForCapture && !currentMatForCapture->empty()) outMat = currentMatForCapture->clone();
		}

		Bitmap^ GetCurrentFrame() {
			if (currentFrame != nullptr) {
				return safe_cast<Bitmap^>(currentFrame->Clone());
			}
			return nullptr;
		}

	protected:
		~cameraForm()
		{
			if (components) delete components;
			if (capture != nullptr && capture->isOpened()) {
				capture->release();
				delete capture;
			}
			if (currentFrame != nullptr) delete currentFrame;
			if (currentBoxes) delete currentBoxes;
			if (currentConfidences) delete currentConfidences;
			if (currentMatForCapture) delete currentMatForCapture;
			if (net != nullptr) delete net;
		}

	private:
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->panelMain = (gcnew System::Windows::Forms::Panel());
			this->panelVideo = (gcnew System::Windows::Forms::Panel());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->panelControls = (gcnew System::Windows::Forms::Panel());
			this->labelStatus = (gcnew System::Windows::Forms::Label());
			this->StartButton = (gcnew System::Windows::Forms::Button());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->panelMain->SuspendLayout();
			this->panelVideo->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			this->panelControls->SuspendLayout();
			this->SuspendLayout();
			// 
			// panelMain
			// 
			this->panelMain->BackColor = System::Drawing::Color::FromArgb(30, 30, 35);
			this->panelMain->Controls->Add(this->panelVideo);
			this->panelMain->Controls->Add(this->panelControls);
			this->panelMain->Dock = System::Windows::Forms::DockStyle::Fill;
			this->panelMain->Location = System::Drawing::Point(0, 0);
			this->panelMain->Name = L"panelMain";
			this->panelMain->Padding = System::Windows::Forms::Padding(10);
			this->panelMain->Size = System::Drawing::Size(800, 600);
			this->panelMain->TabIndex = 0;
			// 
			// panelVideo
			// 
			this->panelVideo->BackColor = System::Drawing::Color::FromArgb(40, 40, 45);
			this->panelVideo->Controls->Add(this->pictureBox1);
			this->panelVideo->Dock = System::Windows::Forms::DockStyle::Fill;
			this->panelVideo->Location = System::Drawing::Point(10, 10);
			this->panelVideo->Name = L"panelVideo";
			this->panelVideo->Padding = System::Windows::Forms::Padding(5);
			this->panelVideo->Size = System::Drawing::Size(780, 490);
			this->panelVideo->TabIndex = 0;
			// 
			// pictureBox1
			// 
			this->pictureBox1->BackColor = System::Drawing::Color::Black;
			this->pictureBox1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->pictureBox1->Location = System::Drawing::Point(5, 5);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(770, 480);
			this->pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBox1->TabIndex = 0;
			this->pictureBox1->TabStop = false;
			// 
			// panelControls
			// 
			this->panelControls->BackColor = System::Drawing::Color::FromArgb(45, 45, 50);
			this->panelControls->Controls->Add(this->labelStatus);
			this->panelControls->Controls->Add(this->StartButton);
			this->panelControls->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->panelControls->Location = System::Drawing::Point(10, 500);
			this->panelControls->Name = L"panelControls";
			this->panelControls->Padding = System::Windows::Forms::Padding(15, 10, 15, 10);
			this->panelControls->Size = System::Drawing::Size(780, 90);
			this->panelControls->TabIndex = 1;
			// 
			// labelStatus
			// 
			this->labelStatus->AutoSize = true;
			this->labelStatus->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10, System::Drawing::FontStyle::Bold));
			this->labelStatus->ForeColor = System::Drawing::Color::FromArgb(100, 180, 255);
			this->labelStatus->Location = System::Drawing::Point(200, 23);
			this->labelStatus->Name = L"labelStatus";
			this->labelStatus->Size = System::Drawing::Size(150, 23);
			this->labelStatus->TabIndex = 1;
			this->labelStatus->Text = L"Status: Stopped";
			// 
			// StartButton
			// 
			this->StartButton->BackColor = System::Drawing::Color::FromArgb(0, 122, 204);
			this->StartButton->Cursor = System::Windows::Forms::Cursors::Hand;
			this->StartButton->FlatAppearance->BorderSize = 0;
			this->StartButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->StartButton->Font = (gcnew System::Drawing::Font(L"Segoe UI", 11, System::Drawing::FontStyle::Bold));
			this->StartButton->ForeColor = System::Drawing::Color::White;
			this->StartButton->Location = System::Drawing::Point(20, 15);
			this->StartButton->Name = L"StartButton";
			this->StartButton->Size = System::Drawing::Size(150, 60);
			this->StartButton->TabIndex = 0;
			this->StartButton->Text = L"Start Camera";
			this->StartButton->UseVisualStyleBackColor = false;
			this->StartButton->Click += gcnew System::EventHandler(this, &cameraForm::StartButton_Click);
			// 
			// timer1
			// 
			this->timer1->Interval = 33;
			this->timer1->Tick += gcnew System::EventHandler(this, &cameraForm::timer1_Tick);
			// 
			// cameraForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8.0f, 16.0f);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(800, 600);
			this->Controls->Add(this->panelMain);
			this->MinimumSize = System::Drawing::Size(640, 480);
			this->Name = L"cameraForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"Camera View - YOLO Detection";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &cameraForm::cameraForm_FormClosing);
			this->panelMain->ResumeLayout(false);
			this->panelVideo->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			this->panelControls->ResumeLayout(false);
			this->panelControls->PerformLayout();
			this->ResumeLayout(false);
		}

		void InitializeAI() {
			try {
				net = new cv::dnn::Net(cv::dnn::readNetFromONNX("best.onnx"));
				net->setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
				net->setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
			}
			catch (cv::Exception& e) {
				MessageBox::Show("AI Load Error: " + gcnew String(e.what()), "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
			}
		}

		void DetectAndDraw(cv::Mat& frameForAI, cv::Mat& frameForDisplay) {
			if (net == nullptr || net->empty()) return;

			cv::Mat blob;
			cv::dnn::blobFromImage(frameForAI, blob, 1.0 / 255.0, cv::Size((int)INPUT_WIDTH, (int)INPUT_HEIGHT), cv::Scalar(), true, false);
			net->setInput(blob);

			std::vector<cv::Mat> outputs;
			net->forward(outputs, net->getUnconnectedOutLayersNames());
			if (outputs.empty()) return;

			cv::Mat output_data = outputs[0];
			cv::Mat output_2d = output_data.reshape(1, 5);
			cv::transpose(output_2d, output_data);

			float* data = (float*)output_data.data;
			int rows = output_data.rows;
			int dimensions = output_data.cols;

			std::vector<float> confidences;
			std::vector<cv::Rect> boxes;
			float x_factor = (float)frameForAI.cols / INPUT_WIDTH;
			float y_factor = (float)frameForAI.rows / INPUT_HEIGHT;

			for (int i = 0; i < rows; ++i) {
				float confidence = data[4];
				if (confidence >= aiConfidence) {
					float x = data[0], y = data[1], w = data[2], h = data[3];
					int left = (int)((x - 0.5f * w) * x_factor);
					int top = (int)((y - 0.5f * h) * y_factor);
					int width = (int)(w * x_factor);
					int height = (int)(h * y_factor);
					boxes.push_back(cv::Rect(left, top, width, height));
					confidences.push_back(confidence);
				}
				data += dimensions;
			}

			std::vector<int> nms_result;
			cv::dnn::NMSBoxes(boxes, confidences, aiConfidence, NMS_THRESHOLD, nms_result);

			currentBoxes->clear();
			currentConfidences->clear();

			for (int idx : nms_result) {
				cv::Rect box = boxes[idx];
				currentBoxes->push_back(box);
				currentConfidences->push_back(confidences[idx]);

				cv::rectangle(frameForDisplay, box, cv::Scalar(0, 255, 0), 2);
				std::string label = "Helmet: " + std::to_string((int)(confidences[idx] * 100)) + "%";
				cv::putText(frameForDisplay, label, cv::Point(box.x, box.y - 10),
					cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 2);
			}
		}

		System::Void StartButton_Click(System::Object^ sender, System::EventArgs^ e) {
			if (isStart) {
				isStart = false;
				timer1->Stop();
				StartButton->Text = L"Start Camera";
				StartButton->BackColor = Color::FromArgb(0, 122, 204);
				labelStatus->Text = L"Status: Stopped";
				labelStatus->ForeColor = Color::FromArgb(100, 180, 255);
				
				if (capture != nullptr && capture->isOpened()) {
					capture->release();
				}
				return;
			}

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

			isStart = true;
			timer1->Start();
			StartButton->Text = L"Stop Camera";
			StartButton->BackColor = Color::FromArgb(220, 80, 80);
			labelStatus->Text = L"Status: Running";
			labelStatus->ForeColor = Color::FromArgb(100, 255, 150);
		}

		System::Void timer1_Tick(System::Object^ sender, System::EventArgs^ e) {
			if (capture != nullptr && capture->isOpened()) {
				cv::Mat frame;
				*capture >> frame;

				if (!frame.empty()) {
					cv::flip(frame, frame, 1);
					*currentMatForCapture = frame.clone();

					cv::Mat frameForAI;
					if (isNight || isNightGreen) {
						cv::Mat gray, enhanced;
						cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
						cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(currentClipLimit, cv::Size(8, 8));
						clahe->apply(gray, enhanced);
						if (currentClipLimit > 4.0) {
							cv::GaussianBlur(enhanced, enhanced, cv::Size(3, 3), 0);
						}
						cv::cvtColor(enhanced, frameForAI, cv::COLOR_GRAY2BGR);
					}
					else {
						frameForAI = frame.clone();
					}

					if (isNight) {
						frame = frameForAI.clone();
					}
					else if (isNightGreen) {
						cv::Mat enhanced;
						cv::cvtColor(frameForAI, enhanced, cv::COLOR_BGR2GRAY);
						cv::Mat noise(enhanced.size(), CV_16SC1);
						cv::randn(noise, 0, 25);
						cv::Mat noisyEnhanced;
						enhanced.convertTo(noisyEnhanced, CV_16SC1);
						cv::add(noisyEnhanced, noise, noisyEnhanced);
						noisyEnhanced.convertTo(enhanced, CV_8UC1);
						cv::Mat bgrChannels[3];
						bgrChannels[0] = enhanced * 0.1;
						bgrChannels[1] = enhanced;
						bgrChannels[2] = enhanced * 0.05;
						cv::merge(bgrChannels, 3, frame);
						frame.convertTo(frame, -1, 1.2, 10);
					}

					*currentMatForCapture = frame.clone();
					DetectAndDraw(frameForAI, frame);

					Bitmap^ bmp = MatToBitmap(frame);
					if (currentFrame != nullptr) delete currentFrame;
					currentFrame = safe_cast<Bitmap^>(bmp->Clone());
					if (pictureBox1->Image != nullptr) delete pictureBox1->Image;
					pictureBox1->Image = bmp;
				}
			}
		}

		System::Void cameraForm_FormClosing(System::Object^ sender, FormClosingEventArgs^ e) {
			if (timer1->Enabled) timer1->Stop();
			if (capture != nullptr && capture->isOpened()) {
				capture->release();
			}
		}

		Bitmap^ MatToBitmap(cv::Mat& mat) {
			cv::Mat temp;
			if (!mat.isContinuous()) {
				temp = mat.clone();
			}
			else {
				temp = mat;
			}

			Bitmap^ bitmap = gcnew Bitmap(temp.cols, temp.rows, PixelFormat::Format24bppRgb);
			System::Drawing::Rectangle rect(0, 0, temp.cols, temp.rows);
			BitmapData^ bmpData = bitmap->LockBits(rect, ImageLockMode::WriteOnly, PixelFormat::Format24bppRgb);

			unsigned char* ptrSrc = temp.data;
			unsigned char* ptrDst = (unsigned char*)bmpData->Scan0.ToPointer();
			int srcStride = static_cast<int>(temp.step);
			int dstStride = bmpData->Stride;
			int rowSize = temp.cols * 3;

			for (int y = 0; y < temp.rows; y++) {
				memcpy(ptrDst + y * dstStride, ptrSrc + y * srcStride, rowSize);
			}

			bitmap->UnlockBits(bmpData);
			return bitmap;
		}
	};
}