void merge_compton_files() {
    Merge("./raw_data/DAQ/calibrazione detector/RAW/DataR_calibrazione detector.root","Data_R","ch0 ch1 ch2","merged_data/calibrazione_detector_Na.root");
    Merge("raw_data/DAQ/calibrazione detector americio/RAW/DataR_run americio 1.root","Data_R","ch0 ch1 ch2","merged_data/calibrazione_detector_Am.root");
    Merge("raw_data/DAQ/calibrazione scatterer/RAW/DataR_calibrazione scatterer.root","Data_R","ch0 ch1 ch2","merged_data/calibrazione_scatterer_Na.root");
    Merge("raw_data/DAQ/calibrazione scatterer americio/RAW/DataR_scatterer americio.root","Data_R","ch0 ch1 ch2","merged_data/calibrazione_scatterer_Am.root");
    Merge("raw_data/DAQ/calibrazione tagger/RAW/DataR_prova.root","Data_R","ch0 ch1 ch2","merged_data/calibrazione_tagger_Na.root");
    Merge("raw_data/DAQ/calibrazione tagger americio/RAW/DataR_tagger americio.root","Data_R","ch0 ch1 ch2","merged_data/calibrazione_tagger_Am.root");
    
    Merge("raw_data/DAQ/energia (0 deg, 30 min)/RAW/DataR_energia (0 deg).root","Data_R","ch0 ch1 ch2","merged_data/energia_0deg.root");
    Merge("raw_data/DAQ/energia (30 deg, 30 min)/RAW/DataR_energia (30 deg, 30 min).root","Data_R","ch0 ch1 ch2","merged_data/energia_30deg.root");
    Merge("raw_data/DAQ/energia (50 deg, 20 min)/RAW/DataR_energia 50.root","Data_R","ch0 ch1 ch2","merged_data/energia_50deg.root");
    Merge("raw_data/DAQ/energia (60 deg, 30 min)/RAW/DataR_energia (60 deg, 30 min).root","Data_R","ch0 ch1 ch2","merged_data/energia_60deg.root");
    Merge("raw_data/DAQ/energia (90 deg, 30 min)/RAW/DataR_energia (90 deg, 30 min).root","Data_R","ch0 ch1 ch2","merged_data/energia_90deg.root");
    
    Merge("raw_data/DAQ/fondo senza coincidenza (5 min)/RAW/DataR_fondo senza coincidenza.root","Data_R","ch0 ch1 ch2","merged_data/fondo_no_coinc.root");
    Merge("raw_data/DAQ/fondo in coincidenza (20 min)/RAW/DataR_fondo in coincidenza.root","Data_R","ch0 ch1 ch2","merged_data/fondo_coinc.root");
    
    Merge("raw_data/DAQ/sezione d'urto (100 deg, 20h)/RAW/DataR_sezione d'urto.root","Data_R","ch0 ch1 ch2","merged_data/sezione_urto.root");
    
    Merge("raw_data/DAQ/efficienza/RAW/DataR_efficienza.root","Data_R","ch0 ch1 ch2","merged_data/efficienza.root");
}