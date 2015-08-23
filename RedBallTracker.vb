'RedBallTracker.vb
'
'Emgu CV 3.0.0
'
'put this code in your main form, for example frmMain.vb
'
'add the following components to your form:
'tlpOuter (TableLayoutPanel)
'tlpInner (TableLayoutPanel)
'ibOriginal (Emgu ImageBox)
'ibThresh (Emgu ImageBox)
'btnPauseOrResume (Button)
'txtXYRadius (TextBox)
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

    ' member variables ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    Dim capWebcam As Capture
    Dim blnCapturingInProcess As Boolean = False
    
    '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    Private Sub frmMain_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        Try
            capWebcam = New Capture()                               'associate the capture object to the default webcam
        Catch ex As Exception                                       'catch error if unsuccessful
                                                                    'show error via message box
            MessageBox.Show("unable to read from webcam, error: " + Environment.NewLine + Environment.NewLine + _
                            ex.Message + Environment.NewLine + Environment.NewLine + _
                            "exiting program")
            Environment.Exit(0)                                     'and exit program
        End Try
        
        AddHandler Application.Idle, New EventHandler(AddressOf Me.ProcessFrameAndUpdateGUI)        'add process image function to the application's list of tasks
        blnCapturingInProcess = True
    End Sub
    
    '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    Sub ProcessFrameAndUpdateGUI(sender As Object, arg As EventArgs)
        Dim imgOriginal As Mat

	    imgOriginal = capWebcam.QueryFrame()							'get next frame from the webcam
	    
        If(imgOriginal Is Nothing)										'if we did not get a frame
		                                                                'show error via message box
            MessageBox.Show("unable to read from webcam" + Environment.NewLine + Environment.NewLine + _
                            "exiting program")
            Environment.Exit(0)                                         'and exit program
	    End If

        Dim imgHSV As New Mat(imgOriginal.Size, DepthType.Cv8U, 3)

        Dim imgThreshLow As New Mat(imgOriginal.Size, DepthType.Cv8U, 1)
        Dim imgThreshHigh As New Mat(imgOriginal.Size, DepthType.Cv8U, 1)

        Dim imgThresh As New Mat(imgOriginal.Size, DepthType.Cv8U, 1)

        CvInvoke.CvtColor(imgOriginal, imgHSV, ColorConversion.Bgr2Hsv)

        CvInvoke.InRange(imgHSV, New ScalarArray(New MCvScalar(0, 155, 155)), New ScalarArray(New MCvScalar(18, 255, 255)), imgThreshLow)
        CvInvoke.InRange(imgHSV, New ScalarArray(New MCvScalar(165, 155, 155)), New ScalarArray(New MCvScalar(179, 255, 255)), imgThreshHigh)
        
        CvInvoke.Add(imgThreshLow, imgThreshHigh, imgThresh)

        CvInvoke.GaussianBlur(imgThresh, imgThresh, New Size(3, 3), 0)
        
        Dim structuringElement As Mat = CvInvoke.GetStructuringElement(ElementShape.Rectangle, New Size(3, 3), New Point(-1, -1))

        CvInvoke.Dilate(imgThresh, imgThresh, structuringElement, New Point(-1, -1), 1, BorderType.Default, New MCvScalar(0, 0, 0))
        CvInvoke.Erode(imgThresh, imgThresh, structuringElement, New Point(-1, -1), 1, BorderType.Default, New MCvScalar(0, 0, 0))
        
        Dim circles As CircleF() = CvInvoke.HoughCircles(imgThresh, HoughType.Gradient, 2.0, imgThresh.Rows / 4, 100, 50, 10, 400)
        
	    For Each circle As CircleF In circles                         'for each circle
		    If(txtXYRadius.Text <> "") Then							'if we are not on the first line in the text box
			    txtXYRadius.AppendText(Environment.NewLine)			'then insert a new line char
		    End If
            
		    txtXYRadius.AppendText("ball position x = " + circle.Center.X.ToString().PadLeft(4) + ", y = " + circle.Center.Y.ToString().PadLeft(4) + ", radius = " + circle.Radius.ToString("###.000").PadLeft(7))
		    txtXYRadius.ScrollToCaret()             'scroll down in text box so most recent line added (at the bottom) will be shown
            
            CvInvoke.Circle(imgOriginal, New Point(CInt(circle.Center.X), CInt(circle.Center.Y)), CInt(circle.Radius), New MCvScalar(0, 0, 255), 2)
            CvInvoke.Circle(imgOriginal, New Point(CInt(circle.Center.X), CInt(circle.Center.Y)), 3, New MCvScalar(0, 255, 0), -1)
            
        Next
        ibOriginal.Image = imgOriginal              'update image boxes on form
        ibThresh.Image = imgThresh                  '
    End Sub
    
    '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    Private Sub btnPauseOrResume_Click(sender As Object, e As EventArgs) Handles btnPauseOrResume.Click
	    If(blnCapturingInProcess = True) Then                                           'if we are currently processing an image, user just choose pause, so . . .
		    RemoveHandler Application.Idle, New EventHandler(AddressOf Me.ProcessFrameAndUpdateGUI)     'remove the process image function from the application's list of tasks
		    blnCapturingInProcess = False																'update flag variable
		    btnPauseOrResume.Text = " Resume "															'update button text
	    Else																			'else if we are not currently processing an image, user just choose resume, so . . .
		    AddHandler Application.Idle, New EventHandler(AddressOf Me.ProcessFrameAndUpdateGUI)        'add the process image function to the application's list of tasks
		    blnCapturingInProcess = True																'update flag variable
		    btnPauseOrResume.Text = " Pause "															'new button will offer pause option
	    End If
    End Sub

End Class
