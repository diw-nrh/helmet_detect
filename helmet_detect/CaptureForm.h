#pragma once
#include <opencv2/opencv.hpp>

namespace helmetdetect {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Drawing::Imaging;

	// Forward declaration only
	ref class cameraForm;

	public ref class CaptureForm : public System::Windows::Forms::Form
	{
	public:
		CaptureForm(void)
		{
			InitializeComponent();
			linkedCameraForm = nullptr;
		}

		// Method to link with cameraForm
		void SetCameraForm(cameraForm^ camForm) {
			linkedCameraForm = camForm;
		}

		// Declare only - implement in MyForm.h after cameraForm is fully defined
		Bitmap^ GetSnapshotFromCamera();

	protected:
		~CaptureForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::SplitContainer^ splitContainer1;
	protected:
	private: System::Windows::Forms::PictureBox^ pictureBox1;
	private: System::Windows::Forms::Button^ btnSnapshot;

	private:
		System::ComponentModel::Container^ components;
	
	public:
		cameraForm^ linkedCameraForm;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->splitContainer1 = (gcnew System::Windows::Forms::SplitContainer());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->btnSnapshot = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splitContainer1))->BeginInit();
			this->splitContainer1->Panel1->SuspendLayout();
			this->splitContainer1->Panel2->SuspendLayout();
			this->splitContainer1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			this->SuspendLayout();
			// 
			// splitContainer1
			// 
			this->splitContainer1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer1->Location = System::Drawing::Point(0, 0);
			this->splitContainer1->Name = L"splitContainer1";
			// 
			// splitContainer1.Panel1
			// 
			this->splitContainer1->Panel1->Controls->Add(this->pictureBox1);
			// 
			// splitContainer1.Panel2
			// 
			this->splitContainer1->Panel2->Controls->Add(this->btnSnapshot);
			this->splitContainer1->Size = System::Drawing::Size(554, 489);
			this->splitContainer1->SplitterDistance = 361;
			this->splitContainer1->TabIndex = 0;
			// 
			// pictureBox1
			// 
			this->pictureBox1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->pictureBox1->Location = System::Drawing::Point(0, 0);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(361, 489);
			this->pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBox1->TabIndex = 0;
			this->pictureBox1->TabStop = false;
			// 
			// btnSnapshot
			// 
			this->btnSnapshot->Location = System::Drawing::Point(46, 27);
			this->btnSnapshot->Name = L"btnSnapshot";
			this->btnSnapshot->Size = System::Drawing::Size(93, 38);
			this->btnSnapshot->TabIndex = 0;
			this->btnSnapshot->Text = L"Snapshot";
			this->btnSnapshot->UseVisualStyleBackColor = true;
			this->btnSnapshot->Click += gcnew System::EventHandler(this, &CaptureForm::btnSnapshot_Click);
			// 
			// CaptureForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(554, 489);
			this->Controls->Add(this->splitContainer1);
			this->Name = L"CaptureForm";
			this->Text = L"Capture Form";
			this->splitContainer1->Panel1->ResumeLayout(false);
			this->splitContainer1->Panel2->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splitContainer1))->EndInit();
			this->splitContainer1->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

	private: System::Void btnSnapshot_Click(System::Object^ sender, System::EventArgs^ e) {
		Bitmap^ snapshot = GetSnapshotFromCamera();
		
		if (snapshot != nullptr) {
			// Display in pictureBox1
			if (pictureBox1->Image != nullptr) {
				delete pictureBox1->Image;
			}
			pictureBox1->Image = snapshot;
		}
		else {
			MessageBox::Show("No frame available. Please start the camera first.", 
				"Snapshot Error", MessageBoxButtons::OK, MessageBoxIcon::Warning);
		}
	}
	};
}