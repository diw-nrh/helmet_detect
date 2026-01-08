#pragma once

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <algorithm>

namespace helmetdetect {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;
    using namespace System::Drawing::Imaging;

    ref class cameraForm;

    public ref class CaptureForm : public System::Windows::Forms::Form
    {
    public:
        CaptureForm(void)
        {
            InitializeComponent();
            linkedCameraForm = nullptr;
            lastBoxes = new std::vector<cv::Rect>();
            lastConfidences = new std::vector<float>();
            lastCapturedMat = new cv::Mat();
        }

    protected:
        ~CaptureForm()
        {
            if (components) delete components;
            if (lastBoxes) delete lastBoxes;
            if (lastConfidences) delete lastConfidences;
            if (lastCapturedMat) delete lastCapturedMat;
        }

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

        Bitmap^ GetSnapshotFromCamera();

    private:
        System::Windows::Forms::Panel^ panelMain;
        System::Windows::Forms::Panel^ panelLeft;
        System::Windows::Forms::Panel^ panelRight;
        System::Windows::Forms::Panel^ panelButtons;
        System::Windows::Forms::Panel^ panelInfo;
        System::Windows::Forms::PictureBox^ pictureBox1;
        System::Windows::Forms::Button^ btnSnapshot;
        System::Windows::Forms::Button^ btnClear;
        System::Windows::Forms::Label^ labelTitle;
        System::Windows::Forms::Label^ labelInfo;
        System::Windows::Forms::Label^ labelHelp;
        System::ComponentModel::Container^ components;

        void InitializeComponent(void)
        {
            this->panelMain = (gcnew System::Windows::Forms::Panel());
            this->panelRight = (gcnew System::Windows::Forms::Panel());
            this->panelInfo = (gcnew System::Windows::Forms::Panel());
            this->labelHelp = (gcnew System::Windows::Forms::Label());
            this->labelInfo = (gcnew System::Windows::Forms::Label());
            this->panelButtons = (gcnew System::Windows::Forms::Panel());
            this->btnClear = (gcnew System::Windows::Forms::Button());
            this->btnSnapshot = (gcnew System::Windows::Forms::Button());
            this->labelTitle = (gcnew System::Windows::Forms::Label());
            this->panelLeft = (gcnew System::Windows::Forms::Panel());
            this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
            this->panelMain->SuspendLayout();
            this->panelRight->SuspendLayout();
            this->panelInfo->SuspendLayout();
            this->panelButtons->SuspendLayout();
            this->panelLeft->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
            this->SuspendLayout();
            // 
            // panelMain
            // 
            this->panelMain->BackColor = System::Drawing::Color::FromArgb(30, 30, 35);
            this->panelMain->Controls->Add(this->panelRight);
            this->panelMain->Controls->Add(this->panelLeft);
            this->panelMain->Dock = System::Windows::Forms::DockStyle::Fill;
            this->panelMain->Location = System::Drawing::Point(0, 0);
            this->panelMain->Name = L"panelMain";
            this->panelMain->Padding = System::Windows::Forms::Padding(10);
            this->panelMain->Size = System::Drawing::Size(900, 600);
            this->panelMain->TabIndex = 0;
            // 
            // panelRight
            // 
            this->panelRight->BackColor = System::Drawing::Color::FromArgb(45, 45, 50);
            this->panelRight->Controls->Add(this->panelInfo);
            this->panelRight->Controls->Add(this->panelButtons);
            this->panelRight->Dock = System::Windows::Forms::DockStyle::Right;
            this->panelRight->Location = System::Drawing::Point(610, 10);
            this->panelRight->Name = L"panelRight";
            this->panelRight->Padding = System::Windows::Forms::Padding(15);
            this->panelRight->Size = System::Drawing::Size(280, 580);
            this->panelRight->TabIndex = 1;
            // 
            // panelInfo
            // 
            this->panelInfo->BackColor = System::Drawing::Color::FromArgb(55, 55, 60);
            this->panelInfo->Controls->Add(this->labelHelp);
            this->panelInfo->Controls->Add(this->labelInfo);
            this->panelInfo->Dock = System::Windows::Forms::DockStyle::Fill;
            this->panelInfo->Location = System::Drawing::Point(15, 195);
            this->panelInfo->Name = L"panelInfo";
            this->panelInfo->Padding = System::Windows::Forms::Padding(15);
            this->panelInfo->Size = System::Drawing::Size(250, 370);
            this->panelInfo->TabIndex = 1;
            // 
            // labelHelp
            // 
            this->labelHelp->Dock = System::Windows::Forms::DockStyle::Top;
            this->labelHelp->Font = (gcnew System::Drawing::Font(L"Segoe UI", 9));
            this->labelHelp->ForeColor = System::Drawing::Color::LightGray;
            this->labelHelp->Location = System::Drawing::Point(15, 65);
            this->labelHelp->Name = L"labelHelp";
            this->labelHelp->Size = System::Drawing::Size(220, 280);
            this->labelHelp->TabIndex = 1;
            this->labelHelp->Text = L"How to use:\r\n\r\n1. Click Snapshot to capture\r\n\r\n2. Double-click helmet for details\r\n\r\n3. Click Clear to remove";
            // 
            // labelInfo
            // 
            this->labelInfo->Dock = System::Windows::Forms::DockStyle::Top;
            this->labelInfo->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10, System::Drawing::FontStyle::Bold));
            this->labelInfo->ForeColor = System::Drawing::Color::FromArgb(100, 180, 255);
            this->labelInfo->Location = System::Drawing::Point(15, 15);
            this->labelInfo->Name = L"labelInfo";
            this->labelInfo->Size = System::Drawing::Size(220, 50);
            this->labelInfo->TabIndex = 0;
            this->labelInfo->Text = L"Information";
            this->labelInfo->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
            // 
            // panelButtons
            // 
            this->panelButtons->Controls->Add(this->btnClear);
            this->panelButtons->Controls->Add(this->btnSnapshot);
            this->panelButtons->Controls->Add(this->labelTitle);
            this->panelButtons->Dock = System::Windows::Forms::DockStyle::Top;
            this->panelButtons->Location = System::Drawing::Point(15, 15);
            this->panelButtons->Name = L"panelButtons";
            this->panelButtons->Size = System::Drawing::Size(250, 180);
            this->panelButtons->TabIndex = 0;
            // 
            // btnClear
            // 
            this->btnClear->BackColor = System::Drawing::Color::FromArgb(220, 80, 80);
            this->btnClear->Cursor = System::Windows::Forms::Cursors::Hand;
            this->btnClear->FlatAppearance->BorderSize = 0;
            this->btnClear->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            this->btnClear->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10, System::Drawing::FontStyle::Bold));
            this->btnClear->ForeColor = System::Drawing::Color::White;
            this->btnClear->Location = System::Drawing::Point(15, 125);
            this->btnClear->Name = L"btnClear";
            this->btnClear->Size = System::Drawing::Size(220, 45);
            this->btnClear->TabIndex = 2;
            this->btnClear->Text = L"Clear Snapshot";
            this->btnClear->UseVisualStyleBackColor = false;
            this->btnClear->Click += gcnew System::EventHandler(this, &CaptureForm::btnClear_Click);
            // 
            // btnSnapshot
            // 
            this->btnSnapshot->BackColor = System::Drawing::Color::FromArgb(0, 122, 204);
            this->btnSnapshot->Cursor = System::Windows::Forms::Cursors::Hand;
            this->btnSnapshot->FlatAppearance->BorderSize = 0;
            this->btnSnapshot->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            this->btnSnapshot->Font = (gcnew System::Drawing::Font(L"Segoe UI", 10, System::Drawing::FontStyle::Bold));
            this->btnSnapshot->ForeColor = System::Drawing::Color::White;
            this->btnSnapshot->Location = System::Drawing::Point(15, 65);
            this->btnSnapshot->Name = L"btnSnapshot";
            this->btnSnapshot->Size = System::Drawing::Size(220, 50);
            this->btnSnapshot->TabIndex = 1;
            this->btnSnapshot->Text = L"Take Snapshot";
            this->btnSnapshot->UseVisualStyleBackColor = false;
            this->btnSnapshot->Click += gcnew System::EventHandler(this, &CaptureForm::btnSnapshot_Click);
            // 
            // labelTitle
            // 
            this->labelTitle->Dock = System::Windows::Forms::DockStyle::Top;
            this->labelTitle->Font = (gcnew System::Drawing::Font(L"Segoe UI", 11, System::Drawing::FontStyle::Bold));
            this->labelTitle->ForeColor = System::Drawing::Color::White;
            this->labelTitle->Location = System::Drawing::Point(0, 0);
            this->labelTitle->Name = L"labelTitle";
            this->labelTitle->Size = System::Drawing::Size(250, 50);
            this->labelTitle->TabIndex = 0;
            this->labelTitle->Text = L"Capture Controls";
            this->labelTitle->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            // 
            // panelLeft
            // 
            this->panelLeft->BackColor = System::Drawing::Color::FromArgb(40, 40, 45);
            this->panelLeft->Controls->Add(this->pictureBox1);
            this->panelLeft->Dock = System::Windows::Forms::DockStyle::Fill;
            this->panelLeft->Location = System::Drawing::Point(10, 10);
            this->panelLeft->Name = L"panelLeft";
            this->panelLeft->Padding = System::Windows::Forms::Padding(10);
            this->panelLeft->Size = System::Drawing::Size(880, 580);
            this->panelLeft->TabIndex = 0;
            // 
            // pictureBox1
            // 
            this->pictureBox1->BackColor = System::Drawing::Color::FromArgb(20, 20, 25);
            this->pictureBox1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            this->pictureBox1->Cursor = System::Windows::Forms::Cursors::Cross;
            this->pictureBox1->Dock = System::Windows::Forms::DockStyle::Fill;
            this->pictureBox1->Location = System::Drawing::Point(10, 10);
            this->pictureBox1->Name = L"pictureBox1";
            this->pictureBox1->Size = System::Drawing::Size(580, 560);
            this->pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
            this->pictureBox1->TabIndex = 0;
            this->pictureBox1->TabStop = false;
            this->pictureBox1->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &CaptureForm::pictureBox1_MouseDoubleClick);
            // 
            // CaptureForm
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(8.0f, 16.0f);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(900, 600);
            this->Controls->Add(this->panelMain);
            this->MinimumSize = System::Drawing::Size(800, 500);
            this->Name = L"CaptureForm";
            this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
            this->Text = L"Helmet Detection - Capture";
            this->panelMain->ResumeLayout(false);
            this->panelRight->ResumeLayout(false);
            this->panelInfo->ResumeLayout(false);
            this->panelButtons->ResumeLayout(false);
            this->panelLeft->ResumeLayout(false);
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
            this->ResumeLayout(false);
        }

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

            double maxDiff = (std::max)({abs(r - g), abs(g - b), abs(r - b)});
            
            if (r < 140 && g < 140 && b < 140 && maxDiff < 20) return "Black";
            if (r > 180 && g > 180 && b > 180 && maxDiff < 25) return "White";
            if (r > 140 && r > g + 40 && r > b + 40) return "Red";
            if (b > 140 && b > r + 40 && b > g + 40) return "Blue";
            if (r > 140 && g > 140 && b < 100 && abs(r - g) < 30) return "Yellow";
            if (g > 140 && g > r + 40 && g > b + 40) return "Green";

            return "Other";
        }

        System::Void btnSnapshot_Click(System::Object^ sender, System::EventArgs^ e);
        
        System::Void btnClear_Click(System::Object^ sender, System::EventArgs^ e) 
        {
            if (pictureBox1->Image != nullptr) {
                delete pictureBox1->Image;
                pictureBox1->Image = nullptr;
            }
            
            lastBoxes->clear();
            lastConfidences->clear();
            if (lastCapturedMat && !lastCapturedMat->empty()) {
                lastCapturedMat->release();
            }
        }
        
        System::Void pictureBox1_MouseDoubleClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
    };
}
