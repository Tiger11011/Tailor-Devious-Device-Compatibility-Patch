param([Parameter(Mandatory=$true)][string]$OutputPath)
Add-Type -AssemblyName System.Drawing
$promptBitmap = New-Object System.Drawing.Bitmap 620,210
$promptGraphics = [System.Drawing.Graphics]::FromImage($promptBitmap)
$promptGraphics.Clear([System.Drawing.Color]::FromArgb(38,38,42))
$promptGraphics.TextRenderingHint = [System.Drawing.Text.TextRenderingHint]::AntiAliasGridFit
$promptBrush = [System.Drawing.Brushes]::White
$promptChineseFont = New-Object System.Drawing.Font 'Microsoft YaHei',25
$promptEnglishFont = New-Object System.Drawing.Font 'Segoe UI',22
$promptFormat = New-Object System.Drawing.StringFormat
$promptFormat.Alignment = [System.Drawing.StringAlignment]::Center
$promptFormat.LineAlignment = [System.Drawing.StringAlignment]::Center
$promptGraphics.DrawString('你用什么语言？',$promptChineseFont,$promptBrush,[System.Drawing.RectangleF]::new(10,35,600,65),$promptFormat)
$promptGraphics.DrawString('What language do you use?',$promptEnglishFont,$promptBrush,[System.Drawing.RectangleF]::new(10,105,600,60),$promptFormat)
$promptBitmap.Save($OutputPath,[System.Drawing.Imaging.ImageFormat]::Png)
$promptFormat.Dispose()
$promptChineseFont.Dispose()
$promptEnglishFont.Dispose()
$promptGraphics.Dispose()
$promptBitmap.Dispose()
