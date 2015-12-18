namespace FrameGrabberApp
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose( bool disposing )
        {
            if ( disposing && ( components != null ) )
            {
                components.Dispose( );
            }
            base.Dispose( disposing );
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent( )
        {
            this.components = new System.ComponentModel.Container();
            this.label1 = new System.Windows.Forms.Label();
            this.cmbDevices = new System.Windows.Forms.ComboBox();
            this.btnCapture = new System.Windows.Forms.Button();
            this.panel1 = new System.Windows.Forms.Panel();
            this.pbVideo = new System.Windows.Forms.PictureBox();
            this.toolTip = new System.Windows.Forms.ToolTip(this.components);
            this.btnPicture = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.cmbPort = new System.Windows.Forms.ComboBox();
            this.label3 = new System.Windows.Forms.Label();
            this.tbOutputFolder = new System.Windows.Forms.TextBox();
            this.btnBrowse = new System.Windows.Forms.Button();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.pictureBox2 = new System.Windows.Forms.PictureBox();
            this.pictureBox3 = new System.Windows.Forms.PictureBox();
            this.pictureBox4 = new System.Windows.Forms.PictureBox();
            this.pictureBox5 = new System.Windows.Forms.PictureBox();
            this.pname1 = new System.Windows.Forms.Label();
            this.presult1 = new System.Windows.Forms.Label();
            this.pname2 = new System.Windows.Forms.Label();
            this.pname3 = new System.Windows.Forms.Label();
            this.pname4 = new System.Windows.Forms.Label();
            this.pname5 = new System.Windows.Forms.Label();
            this.presult3 = new System.Windows.Forms.Label();
            this.presult2 = new System.Windows.Forms.Label();
            this.presult4 = new System.Windows.Forms.Label();
            this.presult5 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.cmbResolution = new System.Windows.Forms.ComboBox();
            this.panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pbVideo)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox2)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox3)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox4)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox5)).BeginInit();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 494);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(79, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Capture source";
            // 
            // cmbDevices
            // 
            this.cmbDevices.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbDevices.FormattingEnabled = true;
            this.cmbDevices.Location = new System.Drawing.Point(91, 490);
            this.cmbDevices.Name = "cmbDevices";
            this.cmbDevices.Size = new System.Drawing.Size(176, 21);
            this.cmbDevices.TabIndex = 1;
            this.cmbDevices.SelectedIndexChanged += new System.EventHandler(this.cmbDevices_SelectedIndexChanged);
            // 
            // btnCapture
            // 
            this.btnCapture.Location = new System.Drawing.Point(604, 530);
            this.btnCapture.Name = "btnCapture";
            this.btnCapture.Size = new System.Drawing.Size(86, 23);
            this.btnCapture.TabIndex = 6;
            this.btnCapture.TabStop = false;
            this.btnCapture.Text = "Start capture";
            this.btnCapture.UseVisualStyleBackColor = true;
            this.btnCapture.Click += new System.EventHandler(this.btnCapture_Click);
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.pbVideo);
            this.panel1.Location = new System.Drawing.Point(1, 2);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(780, 472);
            this.panel1.TabIndex = 8;
            // 
            // pbVideo
            // 
            this.pbVideo.Location = new System.Drawing.Point(14, 10);
            this.pbVideo.Name = "pbVideo";
            this.pbVideo.Size = new System.Drawing.Size(757, 444);
            this.pbVideo.TabIndex = 1;
            this.pbVideo.TabStop = false;
            // 
            // toolTip
            // 
            this.toolTip.AutoPopDelay = 5000;
            this.toolTip.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            this.toolTip.InitialDelay = 100;
            this.toolTip.ReshowDelay = 100;
            // 
            // btnPicture
            // 
            this.btnPicture.Location = new System.Drawing.Point(696, 530);
            this.btnPicture.Name = "btnPicture";
            this.btnPicture.Size = new System.Drawing.Size(85, 23);
            this.btnPicture.TabIndex = 9;
            this.btnPicture.TabStop = false;
            this.btnPicture.Text = "Take picture";
            this.btnPicture.UseVisualStyleBackColor = true;
            this.btnPicture.Click += new System.EventHandler(this.btnPicture_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(289, 494);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(52, 13);
            this.label2.TabIndex = 10;
            this.label2.Text = "COM port";
            // 
            // cmbPort
            // 
            this.cmbPort.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbPort.FormattingEnabled = true;
            this.cmbPort.Items.AddRange(new object[] {
            "COM1",
            "COM2",
            "COM3",
            "COM4",
            "COM5",
            "COM6",
            "COM7",
            "COM8"});
            this.cmbPort.Location = new System.Drawing.Point(343, 490);
            this.cmbPort.Name = "cmbPort";
            this.cmbPort.Size = new System.Drawing.Size(64, 21);
            this.cmbPort.TabIndex = 11;
            this.cmbPort.SelectedIndexChanged += new System.EventHandler(this.cmbPort_SelectedIndexChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(431, 494);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(68, 13);
            this.label3.TabIndex = 12;
            this.label3.Text = "Output folder";
            // 
            // tbOutputFolder
            // 
            this.tbOutputFolder.Location = new System.Drawing.Point(502, 491);
            this.tbOutputFolder.Name = "tbOutputFolder";
            this.tbOutputFolder.Size = new System.Drawing.Size(252, 20);
            this.tbOutputFolder.TabIndex = 13;
            this.tbOutputFolder.Text = "C:\\Output";
            // 
            // btnBrowse
            // 
            this.btnBrowse.Location = new System.Drawing.Point(753, 490);
            this.btnBrowse.Name = "btnBrowse";
            this.btnBrowse.Size = new System.Drawing.Size(28, 22);
            this.btnBrowse.TabIndex = 14;
            this.btnBrowse.Text = "...";
            this.btnBrowse.UseVisualStyleBackColor = true;
            this.btnBrowse.Click += new System.EventHandler(this.btnBrowse_Click);
            // 
            // pictureBox1
            // 
            this.pictureBox1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pictureBox1.Location = new System.Drawing.Point(787, 12);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(144, 86);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox1.TabIndex = 15;
            this.pictureBox1.TabStop = false;
            // 
            // pictureBox2
            // 
            this.pictureBox2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pictureBox2.Location = new System.Drawing.Point(787, 122);
            this.pictureBox2.Name = "pictureBox2";
            this.pictureBox2.Size = new System.Drawing.Size(144, 86);
            this.pictureBox2.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox2.TabIndex = 16;
            this.pictureBox2.TabStop = false;
            // 
            // pictureBox3
            // 
            this.pictureBox3.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pictureBox3.Location = new System.Drawing.Point(787, 232);
            this.pictureBox3.Name = "pictureBox3";
            this.pictureBox3.Size = new System.Drawing.Size(144, 86);
            this.pictureBox3.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox3.TabIndex = 17;
            this.pictureBox3.TabStop = false;
            // 
            // pictureBox4
            // 
            this.pictureBox4.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pictureBox4.Location = new System.Drawing.Point(787, 342);
            this.pictureBox4.Name = "pictureBox4";
            this.pictureBox4.Size = new System.Drawing.Size(144, 86);
            this.pictureBox4.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox4.TabIndex = 18;
            this.pictureBox4.TabStop = false;
            // 
            // pictureBox5
            // 
            this.pictureBox5.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pictureBox5.Location = new System.Drawing.Point(787, 452);
            this.pictureBox5.Name = "pictureBox5";
            this.pictureBox5.Size = new System.Drawing.Size(144, 86);
            this.pictureBox5.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox5.TabIndex = 19;
            this.pictureBox5.TabStop = false;
            // 
            // pname1
            // 
            this.pname1.AutoSize = true;
            this.pname1.Location = new System.Drawing.Point(787, 102);
            this.pname1.Name = "pname1";
            this.pname1.Size = new System.Drawing.Size(127, 13);
            this.pname1.TabIndex = 20;
            this.pname1.Text = "2015-12-08-03-27-38.png";
            // 
            // presult1
            // 
            this.presult1.AutoSize = true;
            this.presult1.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.presult1.ForeColor = System.Drawing.Color.Green;
            this.presult1.Location = new System.Drawing.Point(915, 100);
            this.presult1.Name = "presult1";
            this.presult1.Size = new System.Drawing.Size(18, 17);
            this.presult1.TabIndex = 21;
            this.presult1.Text = "V";
            // 
            // pname2
            // 
            this.pname2.AutoSize = true;
            this.pname2.Location = new System.Drawing.Point(787, 212);
            this.pname2.Name = "pname2";
            this.pname2.Size = new System.Drawing.Size(127, 13);
            this.pname2.TabIndex = 22;
            this.pname2.Text = "2015-12-08-03-27-38.png";
            // 
            // pname3
            // 
            this.pname3.AutoSize = true;
            this.pname3.Location = new System.Drawing.Point(787, 322);
            this.pname3.Name = "pname3";
            this.pname3.Size = new System.Drawing.Size(127, 13);
            this.pname3.TabIndex = 23;
            this.pname3.Text = "2015-12-08-03-27-38.png";
            // 
            // pname4
            // 
            this.pname4.AutoSize = true;
            this.pname4.Location = new System.Drawing.Point(787, 432);
            this.pname4.Name = "pname4";
            this.pname4.Size = new System.Drawing.Size(127, 13);
            this.pname4.TabIndex = 23;
            this.pname4.Text = "2015-12-08-03-27-38.png";
            // 
            // pname5
            // 
            this.pname5.AutoSize = true;
            this.pname5.Location = new System.Drawing.Point(787, 542);
            this.pname5.Name = "pname5";
            this.pname5.Size = new System.Drawing.Size(127, 13);
            this.pname5.TabIndex = 24;
            this.pname5.Text = "2015-12-08-03-27-38.png";
            // 
            // presult3
            // 
            this.presult3.AutoSize = true;
            this.presult3.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.presult3.ForeColor = System.Drawing.Color.Green;
            this.presult3.Location = new System.Drawing.Point(915, 320);
            this.presult3.Name = "presult3";
            this.presult3.Size = new System.Drawing.Size(18, 17);
            this.presult3.TabIndex = 25;
            this.presult3.Text = "V";
            // 
            // presult2
            // 
            this.presult2.AutoSize = true;
            this.presult2.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.presult2.ForeColor = System.Drawing.Color.Green;
            this.presult2.Location = new System.Drawing.Point(915, 210);
            this.presult2.Name = "presult2";
            this.presult2.Size = new System.Drawing.Size(18, 17);
            this.presult2.TabIndex = 22;
            this.presult2.Text = "V";
            // 
            // presult4
            // 
            this.presult4.AutoSize = true;
            this.presult4.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.presult4.ForeColor = System.Drawing.Color.Green;
            this.presult4.Location = new System.Drawing.Point(915, 430);
            this.presult4.Name = "presult4";
            this.presult4.Size = new System.Drawing.Size(18, 17);
            this.presult4.TabIndex = 22;
            this.presult4.Text = "V";
            // 
            // presult5
            // 
            this.presult5.AutoSize = true;
            this.presult5.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.presult5.ForeColor = System.Drawing.Color.Green;
            this.presult5.Location = new System.Drawing.Point(915, 540);
            this.presult5.Name = "presult5";
            this.presult5.Size = new System.Drawing.Size(18, 17);
            this.presult5.TabIndex = 23;
            this.presult5.Text = "V";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(28, 527);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(57, 13);
            this.label5.TabIndex = 28;
            this.label5.Text = "Resolution";
            // 
            // cmbResolution
            // 
            this.cmbResolution.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbResolution.FormattingEnabled = true;
            this.cmbResolution.Location = new System.Drawing.Point(91, 524);
            this.cmbResolution.Name = "cmbResolution";
            this.cmbResolution.Size = new System.Drawing.Size(80, 21);
            this.cmbResolution.TabIndex = 29;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(943, 565);
            this.Controls.Add(this.cmbResolution);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.presult5);
            this.Controls.Add(this.presult2);
            this.Controls.Add(this.presult4);
            this.Controls.Add(this.presult3);
            this.Controls.Add(this.pname5);
            this.Controls.Add(this.pname4);
            this.Controls.Add(this.pname3);
            this.Controls.Add(this.pname2);
            this.Controls.Add(this.presult1);
            this.Controls.Add(this.pname1);
            this.Controls.Add(this.pictureBox5);
            this.Controls.Add(this.pictureBox4);
            this.Controls.Add(this.pictureBox3);
            this.Controls.Add(this.pictureBox2);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this.btnBrowse);
            this.Controls.Add(this.tbOutputFolder);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.cmbPort);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.btnPicture);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.btnCapture);
            this.Controls.Add(this.cmbDevices);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimumSize = new System.Drawing.Size(535, 440);
            this.Name = "MainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Frame Grabber Application v15";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.panel1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pbVideo)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox2)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox3)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox4)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox5)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ComboBox cmbDevices;
        private System.Windows.Forms.Button btnCapture;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.ToolTip toolTip;
        private System.Windows.Forms.Button btnPicture;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox cmbPort;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox tbOutputFolder;
        private System.Windows.Forms.Button btnBrowse;
        private System.Windows.Forms.PictureBox pbVideo;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.PictureBox pictureBox2;
        private System.Windows.Forms.PictureBox pictureBox3;
        private System.Windows.Forms.PictureBox pictureBox4;
        private System.Windows.Forms.PictureBox pictureBox5;
        private System.Windows.Forms.Label pname1;
        private System.Windows.Forms.Label presult1;
        private System.Windows.Forms.Label pname2;
        private System.Windows.Forms.Label pname3;
        private System.Windows.Forms.Label pname4;
        private System.Windows.Forms.Label pname5;
        private System.Windows.Forms.Label presult3;
        private System.Windows.Forms.Label presult2;
        private System.Windows.Forms.Label presult4;
        private System.Windows.Forms.Label presult5;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.ComboBox cmbResolution;
    }
}

