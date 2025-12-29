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
		cameraForm(void)
		{
			InitializeComponent();
			capture = nullptr;
			isStart = false;
			currentFrame = nullptr;

			// 🔥 เรียกโหลด AI ตอนเริ่มฟอร์ม
			InitializeAI();
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
			// 🔥 ล้างสมอง AI
			if (net != nullptr) delete net;
		}

	private: System::Windows::Forms::PictureBox^ pictureBox1;
	private: System::Windows::Forms::Button^ StartButton;
	private: System::Windows::Forms::Timer^ timer1;
	private: System::ComponentModel::IContainer^ components;

	private:
		cv::VideoCapture* capture;
		bool isStart;
		Bitmap^ currentFrame;

		// --- 🔥 ตัวแปรสำหรับ AI (YOLO) ---
		cv::dnn::Net* net;
		const float INPUT_WIDTH = 640.0;
		const float INPUT_HEIGHT = 640.0;
		const float SCORE_THRESHOLD = 0.5;
		const float NMS_THRESHOLD = 0.45;

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
			this->Text = L"Camera View (YOLO11)";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &cameraForm::cameraForm_FormClosing);
			this->Load += gcnew System::EventHandler(this, &cameraForm::cameraForm_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

		// ============================================
		// 🔥 ส่วนที่ 1: โหลดโมเดล
		// ============================================
	private: void InitializeAI() {
		try {
			// ตรวจสอบว่ามีไฟล์ best.onnx อยู่ข้างไฟล์ .exe หรือยัง
			net = new cv::dnn::Net(cv::dnn::readNetFromONNX("best.onnx"));

			// ถ้ามี GPU ให้เปลี่ยนเป็น CUDA ตรงนี้
			net->setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
			net->setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
		}
		catch (cv::Exception& e) {
			MessageBox::Show("Error Loading AI: " + gcnew String(e.what()));
		}
	}

		   // ============================================
		   // 🔥 ส่วนที่ 2: ฟังก์ชันตรวจจับ (Logic หลัก)
		   // ============================================
	private: void DetectAndDraw(cv::Mat& frame) {
		if (net == nullptr || net->empty()) return;

		// 1. Prepare Input
		cv::Mat blob;
		cv::dnn::blobFromImage(frame, blob, 1.0 / 255.0, cv::Size(INPUT_WIDTH, INPUT_HEIGHT), cv::Scalar(), true, false);
		net->setInput(blob);

		// 2. Inference
		std::vector<cv::Mat> outputs;
		net->forward(outputs, net->getUnconnectedOutLayersNames());

		// 3. Process Output
		if (outputs.empty()) return;
		cv::Mat output_data = outputs[0];

		// Reshape [1, 5, 8400] -> [5, 8400] -> Transpose [8400, 5]
		cv::Mat output_2d = output_data.reshape(1, 5);
		cv::transpose(output_2d, output_data);

		float* data = (float*)output_data.data;
		int rows = output_data.rows;
		int dimensions = output_data.cols; // ควรเป็น 5

		std::vector<float> confidences;
		std::vector<cv::Rect> boxes;

		float x_factor = (float)frame.cols / INPUT_WIDTH;
		float y_factor = (float)frame.rows / INPUT_HEIGHT;

		for (int i = 0; i < rows; ++i) {
			float confidence = data[4]; // Index 4 คือ Score ของ Helmet

			if (confidence >= SCORE_THRESHOLD) {
				float x = data[0];
				float y = data[1];
				float w = data[2];
				float h = data[3];

				int left = int((x - 0.5 * w) * x_factor);
				int top = int((y - 0.5 * h) * y_factor);
				int width = int(w * x_factor);
				int height = int(h * y_factor);

				boxes.push_back(cv::Rect(left, top, width, height));
				confidences.push_back(confidence);
			}
			data += dimensions;
		}

		// 4. NMS
		std::vector<int> nms_result;
		cv::dnn::NMSBoxes(boxes, confidences, SCORE_THRESHOLD, NMS_THRESHOLD, nms_result);

		// 5. Draw Results
		for (int idx : nms_result) {
			cv::Rect box = boxes[idx];
			cv::rectangle(frame, box, cv::Scalar(0, 255, 0), 2); // สีเขียว

			std::string label = "Helmet: " + std::to_string((int)(confidences[idx] * 100)) + "%";
			cv::putText(frame, label, cv::Point(box.x, box.y - 10), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 2);
		}
	}

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

				// 🔥 แทรกตรงนี้: ส่งภาพไปให้ AI ตรวจจับก่อนแสดงผล
				DetectAndDraw(frame);

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
	private: System::Void cameraForm_Load(System::Object^ sender, System::EventArgs^ e) {
	}
	};
}