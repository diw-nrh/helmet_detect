#pragma once

#include <opencv2/opencv.hpp>
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

    // forward declaration
    ref class cameraForm;

    public ref class CaptureForm : public System::Windows::Forms::Form
    {
    public:
        CaptureForm(void)
        {
            InitializeComponent();

            linkedCameraForm = nullptr;

            // allocate native memory
            lastBoxes = new std::vector<cv::Rect>();
            lastConfidences = new std::vector<float>();
            lastCapturedMat = new cv::Mat();
        }

    protected:
        ~CaptureForm()
        {
            if (components)
                delete components;

            // free native memory
            if (lastBoxes) delete lastBoxes;
            if (lastConfidences) delete lastConfidences;
            if (lastCapturedMat) delete lastCapturedMat;
        }

        // ===== logic / data =====
    private:
        std::vector<cv::Rect>* lastBoxes;
        std::vector<float>* lastConfidences;
        cv::Mat* lastCapturedMat;

    public:
        cameraForm^ linkedCameraForm;

        void SetCameraForm(cameraForm^ camForm)
        {
            linkedCameraForm = camForm;
        }

        // implemented in cameraForm
        Bitmap^ GetSnapshotFromCamera();

        // ===== UI components =====
    private:
        System::Windows::Forms::SplitContainer^ splitContainer1;
        System::Windows::Forms::PictureBox^ pictureBox1;
        System::Windows::Forms::Button^ btnSnapshot;
        System::ComponentModel::Container^ components;

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
            this->splitContainer1->Panel1->Controls->Add(this->pictureBox1);
            this->splitContainer1->Panel2->Controls->Add(this->btnSnapshot);
            this->splitContainer1->Size = System::Drawing::Size(519, 453);
            this->splitContainer1->SplitterDistance = 240;
            this->splitContainer1->TabIndex = 0;
            // 
            // pictureBox1
            // 
            this->pictureBox1->Dock = System::Windows::Forms::DockStyle::Fill;
            this->pictureBox1->Location = System::Drawing::Point(0, 0);
            this->pictureBox1->Name = L"pictureBox1";
            this->pictureBox1->Size = System::Drawing::Size(240, 453);
            this->pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
            this->pictureBox1->TabIndex = 0;
            this->pictureBox1->TabStop = false;
            this->pictureBox1->MouseDoubleClick +=
                gcnew System::Windows::Forms::MouseEventHandler(
                    this, &CaptureForm::pictureBox1_MouseDoubleClick);
            // 
            // btnSnapshot
            // 
            this->btnSnapshot->Location = System::Drawing::Point(155, 37);
            this->btnSnapshot->Name = L"btnSnapshot";
            this->btnSnapshot->Size = System::Drawing::Size(80, 33);
            this->btnSnapshot->TabIndex = 0;
            this->btnSnapshot->Text = L"Snapshot";
            this->btnSnapshot->UseVisualStyleBackColor = true;
            this->btnSnapshot->Click +=
                gcnew System::EventHandler(
                    this, &CaptureForm::btnSnapshot_Click);
            // 
            // CaptureForm
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(519, 453);
            this->Controls->Add(this->splitContainer1);
            this->Name = L"CaptureForm";
            this->Text = L"CaptureForm";
            this->splitContainer1->Panel1->ResumeLayout(false);
            this->splitContainer1->Panel2->ResumeLayout(false);
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splitContainer1))->EndInit();
            this->splitContainer1->ResumeLayout(false);
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
            this->ResumeLayout(false);
        }
#pragma endregion

        // ===== logic methods =====
    private:
        std::string GetHelmetColor(cv::Mat helmetRoi)
        {
            if (helmetRoi.empty()) return "Other";

            int topHeight = (int)(helmetRoi.rows * 0.4);
            if (topHeight <= 0) topHeight = helmetRoi.rows;
            cv::Mat helmetOnly = helmetRoi(cv::Rect(0, 0, helmetRoi.cols, topHeight));

            cv::Scalar avgColor = cv::mean(helmetOnly);
            double b = avgColor[0];
            double g = avgColor[1];
            double r = avgColor[2];

            if (r < 95 && g < 95 && b < 95) return "Black";
            if (r > 130 && g > 130 && b > 130 && abs(r - g) < 25) return "White";
            if (r > 120 && r > g + 45 && r > b + 45) return "Red";
            if (b > 120 && b > r + 45 && b > g + 45) return "Blue";

            return "Other";
        }

    private: System::Void btnSnapshot_Click(System::Object^ sender, System::EventArgs^ e);

    private: System::Void pictureBox1_MouseDoubleClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
    };

}
