from ROOT import *
from sys import argv
import os

NORM=False
LOG=False

gStyle.SetOptStat(False)
gROOT.SetBatch(True)
    
try: fileNames=argv[1:]
except:
    print "No files specified"
    exit()

if not os.path.exists("plots"): os.mkdir("plots")

files=[]
for fileName in fileNames:
    files.append(TFile(fileName))

keys=files[0].GetListOfKeys()

c=TCanvas()
for key in keys:
    if type(files[0].Get(key.GetName()))!=type(TH1F()): continue

    l=TLegend(.49,.69,.89,.89)
    hists=[]
    max=0
    for lp in range(len(files)):
        file=files[lp]
        fileName=fileNames[lp]
        
        file.cd()
        h=file.Get(key.GetName())
        hists.append(h)
        
        if NORM:h.Scale(1./h.Integral())

        if h.GetMaximum()>max: max=h.GetMaximum()

        h.SetLineColor(1+lp)
        l.AddEntry(h,fileNames[lp].split('.')[0],"l")

    for lp in range(len(hists)):
        h=hists[lp]
        
        if lp==0:
            if LOG: h.SetMaximum(100*max)
            else: h.SetMaximum(1.5*max)
            h.Draw()
        else: h.Draw("SAME")
        print h.GetMean()

    l.Draw("SAME")
    c.SaveAs("plots/"+key.GetName()+".pdf")
            
