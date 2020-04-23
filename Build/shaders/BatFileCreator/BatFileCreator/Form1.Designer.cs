namespace BatFileCreator
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.createBtn = new System.Windows.Forms.Button();
            this.fileNameTxtBox = new System.Windows.Forms.TextBox();
            this.driveCmbBox = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.sdkVersionCmbBox = new System.Windows.Forms.ComboBox();
            this.getSdkBtn = new System.Windows.Forms.Button();
            this.exitBtn = new System.Windows.Forms.Button();
            this.label4 = new System.Windows.Forms.Label();
            this.vertShdChBox = new System.Windows.Forms.CheckBox();
            this.fragShdChBox = new System.Windows.Forms.CheckBox();
            this.geomShdChBox = new System.Windows.Forms.CheckBox();
            this.SuspendLayout();
            // 
            // createBtn
            // 
            this.createBtn.Location = new System.Drawing.Point(168, 231);
            this.createBtn.Name = "createBtn";
            this.createBtn.Size = new System.Drawing.Size(100, 23);
            this.createBtn.TabIndex = 2;
            this.createBtn.Text = "Create .bat File";
            this.createBtn.UseVisualStyleBackColor = true;
            this.createBtn.Click += new System.EventHandler(this.createBtn_Click);
            // 
            // fileNameTxtBox
            // 
            this.fileNameTxtBox.Location = new System.Drawing.Point(168, 192);
            this.fileNameTxtBox.Name = "fileNameTxtBox";
            this.fileNameTxtBox.Size = new System.Drawing.Size(121, 20);
            this.fileNameTxtBox.TabIndex = 1;
            // 
            // driveCmbBox
            // 
            this.driveCmbBox.DisplayMember = "A";
            this.driveCmbBox.FormattingEnabled = true;
            this.driveCmbBox.Items.AddRange(new object[] {
            "A",
            "B",
            "C",
            "D",
            "E",
            "F",
            "G",
            "H",
            "I",
            "J",
            "K",
            "L",
            "M",
            "N",
            "O",
            "P",
            "Q",
            "R",
            "S",
            "T",
            "U",
            "V",
            "W",
            "X",
            "Y",
            "Z"});
            this.driveCmbBox.Location = new System.Drawing.Point(168, 71);
            this.driveCmbBox.Name = "driveCmbBox";
            this.driveCmbBox.Size = new System.Drawing.Size(121, 21);
            this.driveCmbBox.TabIndex = 0;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 71);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(32, 13);
            this.label1.TabIndex = 3;
            this.label1.Text = "Drive";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 195);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(66, 13);
            this.label2.TabIndex = 4;
            this.label2.Text = "Name of File";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(12, 30);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(181, 13);
            this.label3.TabIndex = 5;
            this.label3.Text = "Requires Vulkan SDK to be installed!";
            // 
            // sdkVersionCmbBox
            // 
            this.sdkVersionCmbBox.DisplayMember = "A";
            this.sdkVersionCmbBox.FormattingEnabled = true;
            this.sdkVersionCmbBox.Location = new System.Drawing.Point(168, 139);
            this.sdkVersionCmbBox.Name = "sdkVersionCmbBox";
            this.sdkVersionCmbBox.Size = new System.Drawing.Size(121, 21);
            this.sdkVersionCmbBox.TabIndex = 6;
            // 
            // getSdkBtn
            // 
            this.getSdkBtn.Location = new System.Drawing.Point(168, 98);
            this.getSdkBtn.Name = "getSdkBtn";
            this.getSdkBtn.Size = new System.Drawing.Size(100, 23);
            this.getSdkBtn.TabIndex = 7;
            this.getSdkBtn.Text = "Get Vulkan SDKs";
            this.getSdkBtn.UseVisualStyleBackColor = true;
            this.getSdkBtn.Click += new System.EventHandler(this.getSdkBtn_Click);
            // 
            // exitBtn
            // 
            this.exitBtn.Location = new System.Drawing.Point(168, 260);
            this.exitBtn.Name = "exitBtn";
            this.exitBtn.Size = new System.Drawing.Size(75, 23);
            this.exitBtn.TabIndex = 8;
            this.exitBtn.Text = "Exit";
            this.exitBtn.UseVisualStyleBackColor = true;
            this.exitBtn.Click += new System.EventHandler(this.exitBtn_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(12, 142);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(102, 13);
            this.label4.TabIndex = 9;
            this.label4.Text = "Vulkan SDK version";
            // 
            // vertShdChBox
            // 
            this.vertShdChBox.AutoSize = true;
            this.vertShdChBox.Location = new System.Drawing.Point(15, 235);
            this.vertShdChBox.Name = "vertShdChBox";
            this.vertShdChBox.Size = new System.Drawing.Size(93, 17);
            this.vertShdChBox.TabIndex = 10;
            this.vertShdChBox.Text = "Vertex Shader";
            this.vertShdChBox.UseVisualStyleBackColor = true;
            // 
            // fragShdChBox
            // 
            this.fragShdChBox.AutoSize = true;
            this.fragShdChBox.Location = new System.Drawing.Point(15, 258);
            this.fragShdChBox.Name = "fragShdChBox";
            this.fragShdChBox.Size = new System.Drawing.Size(107, 17);
            this.fragShdChBox.TabIndex = 11;
            this.fragShdChBox.Text = "Fragment Shader";
            this.fragShdChBox.UseVisualStyleBackColor = true;
            // 
            // geomShdChBox
            // 
            this.geomShdChBox.AutoSize = true;
            this.geomShdChBox.Location = new System.Drawing.Point(15, 281);
            this.geomShdChBox.Name = "geomShdChBox";
            this.geomShdChBox.Size = new System.Drawing.Size(108, 17);
            this.geomShdChBox.TabIndex = 12;
            this.geomShdChBox.Text = "Geometry Shader";
            this.geomShdChBox.UseVisualStyleBackColor = true;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(301, 371);
            this.Controls.Add(this.geomShdChBox);
            this.Controls.Add(this.fragShdChBox);
            this.Controls.Add(this.vertShdChBox);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.exitBtn);
            this.Controls.Add(this.getSdkBtn);
            this.Controls.Add(this.sdkVersionCmbBox);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.driveCmbBox);
            this.Controls.Add(this.fileNameTxtBox);
            this.Controls.Add(this.createBtn);
            this.Name = "Form1";
            this.Text = "Bat File Generator";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button createBtn;
        private System.Windows.Forms.TextBox fileNameTxtBox;
        private System.Windows.Forms.ComboBox driveCmbBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ComboBox sdkVersionCmbBox;
        private System.Windows.Forms.Button getSdkBtn;
        private System.Windows.Forms.Button exitBtn;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.CheckBox vertShdChBox;
        private System.Windows.Forms.CheckBox fragShdChBox;
        private System.Windows.Forms.CheckBox geomShdChBox;
    }
}

