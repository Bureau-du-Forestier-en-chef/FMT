param (
    # Fichier primaire
    [string]$primaire = 'T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/09_FMT/Modeles_test/08551/PC_9424_U08551_4_Vg1_2023_vSSP03.pri',
    # Scenario
    [string]$scenario = '14_Sc5_Determin_apsp'
)

$ExcelPath = $PSScriptRoot + '\FMT_template.xlsx'
$excel = New-Object -ComObject Excel.Application
$excel.Visible = $false
$workbook = $excel.Workbooks.Open($ExcelPath)
$worksheet = $workbook.worksheets.item('CHARGEMENT')
$worksheet.Cells.Item(1,2) = $primaire
$worksheet.Cells.Item(2,2) = $scenario
$workbook.Save()
$workbook.Close()
$excel.Visible = $true
$excel.ScreenUpdating = $true
#$excel.Application.COMAddIns | ?{$_.Description -eq 'FMTExcel'} |%{$_.Connect=$true}
#ouvrir avec FMTexcel pour l'utilisateur
$wb=$excel.Workbooks.Open($ExcelPath)
$excel.RegisterXLL($PSScriptRoot +"\FMTExcel\bin\Release\FMTExcel-AddIn64.xll")
$ws=$wb.ActiveSheet
$ws.UsedRange.Dirty()
$ws.Calculate()
$wb.Worksheets.Item(2).activate()
$scenws=$wb.ActiveSheet
$scenws.UsedRange.Dirty()
$scenws.Calculate()