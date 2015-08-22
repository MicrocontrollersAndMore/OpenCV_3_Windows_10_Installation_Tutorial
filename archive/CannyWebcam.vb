'CannyWebcam.vb
'
'Emgu CV 3.0.0
'
'put this code in your main form, for example frmMain.vb
'
'add the following components to your form:
'tableLayoutPanel (TableLayoutPanel)
'ibOriginal (Emgu ImageBox)
'ibCanny (Emgu ImageBox)
'
'NOTE: Do NOT copy/paste the entire text of this file into Visual Studio !! It will not work if you do !!
'Follow the video on my YouTube channel to create the project and have Visual Studio write part of the code for you,
'then copy/pase the remaining text as needed

Option Explicit On      'require explicit declaration of variables, this is NOT Python !!
Option Strict On        'restrict implicit data type conversions to only widening conversions

Imports Emgu.CV                 '
Imports Emgu.CV.CvEnum          'usual Emgu Cv imports
Imports Emgu.CV.Structure       '
Imports Emgu.CV.UI              '

'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Public Class frmMain

    ' member variables ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    Dim capWebcam As Capture                        'Capture object to use with webcam

    '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    Private Sub frmMain_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        Try
            capWebcam = New Capture(0)                               'associate the capture object to the default webcam
        Catch ex As Exception                                       'catch error if unsuccessful
                                                                    'show error via message box
            MessageBox.Show("unable to read from webcam, error: " + Environment.NewLine + Environment.NewLine + _
                            ex.Message + Environment.NewLine + Environment.NewLine + _
                            "exiting program")
            Environment.Exit(0)                                     'and exit program
            Return
        End Try
        
        AddHandler Application.Idle, New EventHandler(AddressOf Me.ProcessFrameAndUpdateGUI)        'add process image function to the application's list of tasks
    End Sub

    '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    Sub ProcessFrameAndUpdateGUI(sender As Object, arg As EventArgs)
        Dim imgOriginal As Mat
        
        imgOriginal = capWebcam.QueryFrame()
        
        If (imgOriginal Is Nothing) Then         'if we did not get a frame
            MessageBox.Show("unable to read frame from webcam" + Environment.NewLine + Environment.NewLine + _
                            "exiting program")
            Environment.Exit(0)                                         'and exit program
            Return
        End If
        
        Dim imgGrayscale As New Mat(imgOriginal.Size(), DepthType.Cv8U, 1)
        Dim imgBlurred As New Mat(imgOriginal.Size(), DepthType.Cv8U, 1)
        Dim imgCanny As New Mat(imgOriginal.Size(), DepthType.Cv8U, 1)

        CvInvoke.CvtColor(imgOriginal, imgGrayscale, ColorConversion.Bgr2Gray)

        CvInvoke.GaussianBlur(imgGrayscale, imgBlurred, New Size(5, 5), 1.5)

        CvInvoke.Canny(imgBlurred, imgCanny, 100, 200)

        ibOriginal.Image = imgOriginal              'update image boxes
        ibCanny.Image = imgCanny                    '
    End Sub
    
End Class
