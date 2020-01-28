using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;
using System.Reflection;

namespace BatFileCreator
{
    public partial class Form1 : Form
    {
        string fullDirectory = "unassigned";
        List<string> directories = new List<string>();

        public Form1()
        {
            InitializeComponent();
        }

        private void createBtn_Click(object sender, EventArgs e)
        {
            try
            {
                if (sdkVersionCmbBox.Text == "" || directories.Count <= 0)
                    throw new Exception("Couldn't find Vulkan SDK directory.");
                else if (fileNameTxtBox.Text.Length <= 0)
                    throw new Exception("Enter valid file name.");
                else if (!vertShdChBox.Checked && !fragShdChBox.Checked && !geomShdChBox.Checked)
                    throw new Exception("Select at least 1 shader file to generate.");

                foreach (var dir in directories)
                {
                    for (int j = dir.Length - 1; j > -1; --j)
                    {
                        if (dir[j] == '\\')
                        {
                            int charIndex = j + 1;
                            var subDir = dir.Substring(charIndex, (dir.Length) - charIndex);
                            if (subDir == sdkVersionCmbBox.Text)
                            {
                                fullDirectory = dir;
                            }
                            break;
                        }
                    }
                }

                if (!Directory.Exists(fullDirectory + @"\Bin32"))
                {
                    throw new Exception("Couldn't find 'Bin32' in the '" + fullDirectory + "'directory.");
                }
                if (!File.Exists(fullDirectory + @"\Bin32\glslc.exe"))
                {
                    throw new Exception("Couldn't find 'glslc.exe' in the '" + fullDirectory + @"\Bin32'directory.");
                }

                string batFile = fileNameTxtBox.Text + ".bat";
                string vertFile = fileNameTxtBox.Text + ".vert";
                string fragFile = fileNameTxtBox.Text + ".frag";
                string geomFile = fileNameTxtBox.Text + ".geom";

                if (!Directory.Exists(fileNameTxtBox.Text))
                {
                    Directory.CreateDirectory(fileNameTxtBox.Text);
                }
                else
                {
                    MessageBox.Show("The \n'" + System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location) + @"\" + fileNameTxtBox.Text + "' already exits.");
                    string message = "Do you want to create files in the directory any way?";
                    string caption = "Error Detected in Directory Creation";
                    MessageBoxButtons buttons = MessageBoxButtons.YesNo;
                    DialogResult result;

                    // Displays the MessageBox.
                    result = MessageBox.Show(message, caption, buttons);
                    if (result == System.Windows.Forms.DialogResult.No)
                    {
                        // Closes the parent form.
                        return;
                    }     
                }

                checkBatFile(batFile);
                checkShaderFile(vertShdChBox.Checked, vertFile);
                checkShaderFile(fragShdChBox.Checked, fragFile);
                checkShaderFile(geomShdChBox.Checked, geomFile);

                using (StreamWriter sw = new StreamWriter(fileNameTxtBox.Text + @"\" + batFile))
                {
                    if (vertShdChBox.Checked)
                        sw.WriteLine(driveCmbBox.Text + @":\VulkanSDK\" + sdkVersionCmbBox.Text + @"\Bin32\glslc.exe " + fileNameTxtBox.Text + ".vert -o " + fileNameTxtBox.Text + "Vert.spv");
                    if (fragShdChBox.Checked)
                        sw.WriteLine(driveCmbBox.Text + @":\VulkanSDK\" + sdkVersionCmbBox.Text + @"\Bin32\glslc.exe " + fileNameTxtBox.Text + ".frag -o " + fileNameTxtBox.Text + "Frag.spv");
                    if (geomShdChBox.Checked)
                        sw.WriteLine(driveCmbBox.Text + @":\VulkanSDK\" + sdkVersionCmbBox.Text + @"\Bin32\glslc.exe " + fileNameTxtBox.Text + ".geom -o " + fileNameTxtBox.Text + "Geom.spv");

                    sw.WriteLine("pause");
                }

                String shaderFiles = "";

                if (vertShdChBox.Checked)
                    shaderFiles += "/.vert";
                if (fragShdChBox.Checked)
                    shaderFiles += "/.frag";
                if (geomShdChBox.Checked)
                    shaderFiles += "/.geom";

                MessageBox.Show("Created " + fileNameTxtBox.Text + ".bat" + shaderFiles + " file \nPath:\n'" + System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location) + @"\" + fileNameTxtBox.Text + "'.");
                fileNameTxtBox.Text = "";
            }
            catch (Exception ex)
            {
                string excpt = ex.ToString();
                MessageBox.Show($"The process failed: {ex.Message}");
            }

        }

        private void checkShaderFile(bool checkBoxTicked, string file)
        {
            if (checkBoxTicked)
            {
                if (!File.Exists(fileNameTxtBox.Text + @"\" + file))
                {
                    using (FileStream fs = File.Create(fileNameTxtBox.Text + @"\" + file))
                    {
                        fs.Close();
                    }
                }
                else
                {
                    MessageBox.Show("'" + file + "' already exits.");
                }
            }
        }

        private void checkBatFile(string file)
        {
                if (!File.Exists(fileNameTxtBox.Text + @"\" + file))
                {
                    using (FileStream fs = File.Create(fileNameTxtBox.Text + @"\" + file))
                    {
                        fs.Close();
                    }
                }
                else
                {
                MessageBox.Show("'" + file + "' already exits.");
            }
        }

        private void getSdkBtn_Click(object sender, EventArgs e)
        {
            try
            {
                if (!driveCmbBox.Text.Any<char>())
                {
                    driveCmbBox.Text = "C";
                }
                directories = Directory.EnumerateDirectories(driveCmbBox.Text + @":\VulkanSDK").ToList();
                var dirs = directories.ToArray();
                if (dirs.Count() <= 0)
                {
                    throw new Exception("Cannot find 'VulkanSDK' in " + driveCmbBox.Text +"drive or 'VulkanSDK' has no directories");
                }
                else
                {
                    sdkVersionCmbBox.Items.Clear();
                    for (int i = 0; i < dirs.Length; ++i)
                    {
                        for (int j = dirs[i].Length - 1; j > -1; --j)
                        {
                            if (dirs[i][j] == '\\')
                            {
                                int charIndex = j + 1;
                                dirs[i] = dirs[i].Substring(charIndex, (dirs[i].Length) - charIndex);
                                break;
                            }
                        }
                        sdkVersionCmbBox.Items.Add(dirs[i]);
                    }

                    sdkVersionCmbBox.Text = dirs.First();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"The process failed: {ex.Message}");
            }
        }

        private void exitBtn_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }
    }
}
