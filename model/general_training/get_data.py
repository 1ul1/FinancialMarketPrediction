import os
import json
from time import sleep

STOCKS: list[str] = [
    'A', 'AA', 'AAL', 'AAOI', 'AAON', 'AAP', 'AAPL', 'ABBV', 'ABCB', 'ABCL',
    'ABG', 'ABM', 'ABNB', 'ABR', 'ABT', 'ACAD', 'ACGL', 'ACHR', 'ACI', 'ACIW',
    'ACLS', 'ACM', 'ACMR', 'ACN', 'ACWI', 'ADBE', 'ADC', 'ADI', 'ADM', 'ADMA',
    'ADP', 'ADPT', 'ADSK', 'ADT', 'ADTN', 'ADUS', 'AEE', 'AEHR', 'AEIS', 'AEM',
    'AENT', 'AEO', 'AEP', 'AES', 'AFG', 'AFL', 'AFRM', 'AG', 'AGCO', 'AGG',
    'AGIO', 'AGNC', 'AGRO', 'AI', 'AIG', 'AIQ', 'AIR', 'AIZ', 'AJG', 'AKAM',
    'ALAB', 'ALB', 'ALGM', 'ALGN', 'ALGT', 'ALHC', 'ALIT', 'ALK', 'ALKS', 'ALL',
    'ALLE', 'ALLY', 'ALNY', 'ALSN', 'AMAT', 'AMBA', 'AMC', 'AMCR', 'AMCX', 'AMD',
    'AME', 'AMG', 'AMGN', 'AMH', 'AMKR', 'AMP', 'AMPH', 'AMPL', 'AMR', 'AMRN',
    'AMRX', 'AMT', 'AMTM', 'AMX', 'AMZN', 'AN', 'ANDE', 'ANET', 'ANF', 'ANIP',
    'AON', 'AOS', 'APA', 'APD', 'APH', 'APLD', 'APO', 'APP', 'APPF', 'APPN',
    'APTV', 'ARCB', 'ARDX', 'ARE', 'ARES', 'ARGX', 'ARHS', 'ARI', 'ARKF', 'ARKG',
    'ARKK', 'ARKQ', 'ARKW', 'ARLP', 'ARM', 'ARRY', 'ARVN', 'ARW', 'ASAN', 'ASML',
    'ASND', 'ASO', 'ASTS', 'ATEC', 'ATI', 'ATKR', 'ATO', 'ATR', 'AUR', 'AVA',
    'AVAH', 'AVAV', 'AVB', 'AVGO', 'AVNT', 'AVT', 'AVY', 'AWK', 'AXGN', 'AXON',
    'AXP', 'AXSM', 'AXTA', 'AZN', 'AZO', 'BA', 'BABA', 'BAC', 'BALL', 'BAM',
    'BANC', 'BAX', 'BBAR', 'BBD', 'BBDC', 'BBIO', 'BBSI', 'BBWI', 'BBY', 'BC',
    'BCBP', 'BCC', 'BCO', 'BCPC', 'BCRX', 'BCS', 'BDC', 'BDX', 'BE', 'BEAM',
    'BEKE', 'BEN', 'BFAM', 'BFC', 'BFS', 'BG', 'BGC', 'BGS', 'BHC', 'BHE',
    'BHF', 'BHP', 'BHR', 'BIL', 'BILI', 'BILL', 'BIP', 'BIRK', 'BJ', 'BJRI',
    'BKH', 'BKLN', 'BKNG', 'BKR', 'BKU', 'BL', 'BLBD', 'BLDR', 'BLK', 'BLKB',
    'BLMN', 'BLX', 'BMBL', 'BMI', 'BMO', 'BMRN', 'BMY', 'BND', 'BNO', 'BNTX',
    'BOH', 'BOKF', 'BOOT', 'BOTZ', 'BOX', 'BP', 'BPOP', 'BRC', 'BRK.B', 'BRKR',
    'BRO', 'BROS', 'BRX', 'BRZE', 'BSBR', 'BSX', 'BTI', 'BTU', 'BUD', 'BURL',
    'BUSE', 'BWA', 'BWXT', 'BX', 'BXP', 'BYD', 'C', 'CACC', 'CACI', 'CAG',
    'CAH', 'CAKE', 'CALM', 'CALX', 'CAMT', 'CANE', 'CAR', 'CARG', 'CARR', 'CARS',
    'CART', 'CASY', 'CAT', 'CATY', 'CAVA', 'CB', 'CBOE', 'CBRE', 'CBSH', 'CC',
    'CCBG', 'CCG', 'CCI', 'CCJ', 'CCK', 'CCL', 'CCOI', 'CDNA', 'CDNS', 'CDP',
    'CDW', 'CE', 'CEG', 'CELH', 'CENT', 'CENTA', 'CEVA', 'CEW', 'CF', 'CFG',
    'CFR', 'CGNX', 'CHCO', 'CHD', 'CHDN', 'CHEF', 'CHH', 'CHKP', 'CHRD', 'CHRW',
    'CHTR', 'CHWY', 'CI', 'CIB', 'CIEN', 'CIFR', 'CIGI', 'CINF', 'CL', 'CLBK',
    'CLBT', 'CLDX', 'CLF', 'CLFD', 'CLH', 'CLMT', 'CLOV', 'CLSK', 'CLVT', 'CLX',
    'CM', 'CMC', 'CMCSA', 'CME', 'CMG', 'CMI', 'CMS', 'CNA', 'CNC', 'CNDT',
    'CNH', 'CNI', 'CNK', 'CNM', 'CNMD', 'CNO', 'CNOB', 'CNP', 'CNQ', 'CNX',
    'COCO', 'COF', 'COHR', 'COHU', 'COIN', 'COKE', 'COLB', 'COLL', 'COLM', 'COMT',
    'CON', 'COO', 'COP', 'COR', 'CORN', 'CORT', 'CORZ', 'COST', 'COTY', 'CP',
    'CPAY', 'CPB', 'CPER', 'CPNG', 'CPRT', 'CPT', 'CRCT', 'CRDO', 'CRH', 'CRI',
    'CRK', 'CRL', 'CRM', 'CRNC', 'CRNX', 'CROX', 'CRS', 'CRSP', 'CRTO', 'CRUS',
    'CRWD', 'CSCO', 'CSGP', 'CSIQ', 'CSTM', 'CSX', 'CTAS', 'CTS', 'CTVA', 'CUBE',
    'CURI', 'CVBF', 'CVCO', 'CVE', 'CVLT', 'CVNA', 'CVS', 'CVX', 'CW', 'CWEN',
    'CWK', 'CWST', 'CWT', 'CYRX', 'CYTK', 'CZR', 'D', 'DAKT', 'DAL', 'DAN',
    'DAR', 'DASH', 'DAVE', 'DB', 'DBA', 'DBC', 'DBX', 'DCBO', 'DCI', 'DCOM',
    'DD', 'DDOG', 'DDS', 'DE', 'DECK', 'DELL', 'DEO', 'DFH', 'DFIN', 'DG',
    'DGII', 'DGRO', 'DGRW', 'DGX', 'DHI', 'DHR', 'DIA', 'DIN', 'DINO', 'DIS',
    'DKNG', 'DKS', 'DLB', 'DLO', 'DLR', 'DLTR', 'DNLI', 'DNOW', 'DOC', 'DOCN',
    'DOCS', 'DOCU', 'DOLE', 'DOMO', 'DORM', 'DOV', 'DOW', 'DOX', 'DPZ', 'DRI',
    'DRIV', 'DSGN', 'DT', 'DTE', 'DUK', 'DUOL', 'DVA', 'DVN', 'DXCM', 'EAT',
    'EBAY', 'EBC', 'EC', 'ECH', 'ECHO', 'ECL', 'ECPG', 'ED', 'EDIT', 'EDV',
    'EEFT', 'EEM', 'EFA', 'EFC', 'EFX', 'EG', 'EHTH', 'EIDO', 'EIS', 'EIX',
    'EL', 'ELAN', 'ELF', 'ELV', 'EMB', 'EME', 'EMN', 'EMR', 'ENB', 'ENOV',
    'ENPH', 'ENS', 'ENSG', 'ENTG', 'ENVA', 'EOG', 'EPAM', 'EPD', 'EPHE', 'EPOL',
    'EPR', 'EPU', 'EQH', 'EQIX', 'EQR', 'EQT', 'ERAS', 'ERIC', 'ES', 'ESAB',
    'ESNT', 'ESS', 'ESTC', 'ET', 'ETN', 'ETSY', 'EVGO', 'EVR', 'EVRG', 'EVTC',
    'EW', 'EWBC', 'EWC', 'EWG', 'EWH', 'EWI', 'EWJ', 'EWM', 'EWP', 'EWQ',
    'EWS', 'EWT', 'EWU', 'EWY', 'EWZ', 'EXC', 'EXEL', 'EXLS', 'EXP', 'EXPD',
    'EXPE', 'EXPO', 'EXR', 'EZPW', 'F', 'FAF', 'FAST', 'FBIN', 'FBIZ', 'FBK',
    'FCEL', 'FCFS', 'FCNCA', 'FCX', 'FDN', 'FDS', 'FDX', 'FE', 'FELE', 'FERG',
    'FFBC', 'FFIN', 'FFIV', 'FHN', 'FICO', 'FIP', 'FIS', 'FISV', 'FITB', 'FIVE',
    'FIX', 'FLNC', 'FLOT', 'FLR', 'FMC', 'FMX', 'FNB', 'FND', 'FNV', 'FORM',
    'FOUR', 'FOX', 'FOXA', 'FRME', 'FRO', 'FROG', 'FRPT', 'FRSH', 'FRT', 'FSLR',
    'FSLY', 'FTAI', 'FTI', 'FTNT', 'FUBO', 'FUL', 'FULT', 'FUTU', 'FVRR', 'FXA',
    'FXB', 'FXC', 'FXE', 'FXY', 'G', 'GABC', 'GATX', 'GBCI', 'GCMG', 'GCT',
    'GD', 'GDDY', 'GDRX', 'GE', 'GEF', 'GEHC', 'GEN', 'GENK', 'GEO', 'GEV',
    'GFF', 'GFI', 'GH', 'GHC', 'GILD', 'GILT', 'GIS', 'GKOS', 'GL', 'GLBE',
    'GLD', 'GLNG', 'GLOB', 'GLPI', 'GLW', 'GM', 'GMAB', 'GME', 'GNK', 'GNRC',
    'GNTX', 'GNW', 'GO', 'GOGO', 'GOLF', 'GOOGL', 'GOOS', 'GPC', 'GPK', 'GPN',
    'GRAB', 'GREK', 'GRFS', 'GS', 'GSAT', 'GSG', 'GSK', 'GT', 'GTES', 'GTLB',
    'GTM', 'GWW', 'GXO', 'H', 'HACK', 'HAE', 'HAIN', 'HAL', 'HALO', 'HAS',
    'HASI', 'HAYW', 'HBAN', 'HCA', 'HCC', 'HCI', 'HCSG', 'HD', 'HDB', 'HEI',
    'HGV', 'HHH', 'HIG', 'HII', 'HIMS', 'HL', 'HLI', 'HLIO', 'HLMN', 'HLNE',
    'HLT', 'HMC', 'HNI', 'HNVR', 'HOG', 'HOMB', 'HON', 'HOOD', 'HPE', 'HPQ',
    'HQY', 'HR', 'HRB', 'HRI', 'HRL', 'HRMY', 'HSIC', 'HST', 'HSY', 'HTLD',
    'HTZ', 'HUBB', 'HUBS', 'HUM', 'HUN', 'HWM', 'HXL', 'HYG', 'IAU', 'IBB',
    'IBKR', 'IBM', 'IBN', 'IBOC', 'IBP', 'IBRX', 'ICE', 'ICHR', 'ICL', 'ICLN',
    'ICLR', 'ICSH', 'IDA', 'IDCC', 'IDRV', 'IDXX', 'IEF', 'IEX', 'IFF', 'IGV',
    'IHF', 'IHI', 'IIPR', 'IJJ', 'IJS', 'IJT', 'ILMN', 'IMO', 'INCY', 'INDA',
    'INFY', 'ING', 'INGR', 'INSM', 'INSP', 'INTA', 'INTC', 'INTU', 'INVH', 'IONQ',
    'IOVA', 'IP', 'IPAR', 'IPGP', 'IQ', 'IQV', 'IR', 'IRDM', 'IREN', 'IRM',
    'IRTC', 'ISRG', 'ISTR', 'IT', 'ITB', 'ITOT', 'ITT', 'ITUB', 'ITW', 'IVZ',
    'IWB', 'IWD', 'IWF', 'IWM', 'IWN', 'IWO', 'IWP', 'IWR', 'IWS', 'JACK',
    'JAZZ', 'JBHT', 'JBL', 'JBLU', 'JCI', 'JD', 'JEF', 'JELD', 'JEPI', 'JEPQ',
    'JKHY', 'JKS', 'JNJ', 'JOE', 'JPM', 'JPST', 'JXN', 'KAI', 'KALU', 'KBE',
    'KBH', 'KBR', 'KC', 'KD', 'KDP', 'KEX', 'KEY', 'KFY', 'KGC', 'KHC',
    'KIDS', 'KIM', 'KKR', 'KLAC', 'KLIC', 'KMB', 'KMI', 'KMT', 'KMX', 'KNSA',
    'KNSL', 'KNX', 'KO', 'KOF', 'KR', 'KRC', 'KRE', 'KRUS', 'KSA', 'KTOS',
    'KVUE', 'KWEB', 'LAD', 'LAZ', 'LCID', 'LDOS', 'LEA', 'LECO', 'LEG', 'LEN',
    'LEU', 'LEVI', 'LFMD', 'LFUS', 'LGIH', 'LGND', 'LH', 'LHX', 'LI', 'LII',
    'LILAK', 'LIN', 'LIT', 'LITE', 'LIVN', 'LKQ', 'LLY', 'LMT', 'LNC', 'LNG',
    'LNT', 'LOGI', 'LOPE', 'LOW', 'LPLA', 'LPX', 'LQDA', 'LQDT', 'LRCX', 'LRN',
    'LSAK', 'LSCC', 'LSTR', 'LTH', 'LULU', 'LUNR', 'LUV', 'LVS', 'LW', 'LYB',
    'LYFT', 'LYV', 'M', 'MA', 'MAA', 'MAMA', 'MAN', 'MANH', 'MAR', 'MARA',
    'MAS', 'MAT', 'MATX', 'MBLY', 'MBUU', 'MCD', 'MCFT', 'MCHI', 'MCHP', 'MCK',
    'MCO', 'MDB', 'MDLZ', 'MDT', 'MDU', 'MDY', 'MELI', 'MET', 'META', 'MFG',
    'MGM', 'MHK', 'MHO', 'MIDD', 'MINT', 'MKC', 'MKTX', 'MLAB', 'MLM', 'MMM',
    'MMS', 'MNDY', 'MNKD', 'MNST', 'MNTK', 'MO', 'MOAT', 'MOD', 'MOH', 'MORN',
    'MOS', 'MP', 'MPC', 'MPLX', 'MPWR', 'MRCY', 'MRK', 'MRNA', 'MRVI', 'MRVL',
    'MS', 'MSA', 'MSBI', 'MSCI', 'MSEX', 'MSFT', 'MSI', 'MSTR', 'MTB', 'MTCH',
    'MTD', 'MTDR', 'MTG', 'MTN', 'MTSI', 'MTUM', 'MTZ', 'MU', 'MUB', 'MUR',
    'MWA', 'MXL', 'MYGN', 'NAT', 'NBR', 'NCLH', 'NCNO', 'NDAQ', 'NDSN', 'NEE',
    'NEM', 'NEO', 'NEOG', 'NET', 'NFE', 'NFG', 'NFLX', 'NI', 'NIO', 'NKE',
    'NLY', 'NMR', 'NMRK', 'NNN', 'NOC', 'NOG', 'NOK', 'NOV', 'NOVT', 'NOW',
    'NRG', 'NSC', 'NTAP', 'NTES', 'NTLA', 'NTR', 'NTRA', 'NTRS', 'NU', 'NUE',
    'NUS', 'NVAX', 'NVCR', 'NVDA', 'NVMI', 'NVO', 'NVR', 'NVT', 'NVTS', 'NWSA',
    'NXE', 'NXPI', 'O', 'ODD', 'ODFL', 'OGE', 'OGS', 'OHI', 'OI', 'OII',
    'OKE', 'OKTA', 'OLED', 'OLLI', 'OLN', 'OMC', 'OMCL', 'OMF', 'OMFL', 'ON',
    'ONB', 'ONON', 'OPCH', 'OPK', 'OPRX', 'ORA', 'ORCL', 'ORI', 'ORLY', 'OSCR',
    'OSK', 'OTIS', 'OXM', 'OXY', 'OZK', 'PAA', 'PAAS', 'PACB', 'PAG', 'PAGP',
    'PALL', 'PANW', 'PATH', 'PATK', 'PAYC', 'PAYO', 'PAYX', 'PB', 'PBF', 'PBR',
    'PCAR', 'PCG', 'PCTY', 'PDD', 'PECO', 'PEG', 'PENN', 'PEP', 'PETS', 'PFE',
    'PFG', 'PFGC', 'PFSI', 'PG', 'PGNY', 'PGR', 'PH', 'PHM', 'PI', 'PINS',
    'PK', 'PKG', 'PKOH', 'PL', 'PLD', 'PLMR', 'PLNT', 'PLSE', 'PLTR', 'PLXS',
    'PM', 'PNC', 'PNR', 'PNW', 'PODD', 'POOL', 'POR', 'POWL', 'PPC', 'PPG',
    'PPH', 'PPL', 'PPLT', 'PRAX', 'PRCT', 'PRGS', 'PRI', 'PRIM', 'PRU', 'PSA',
    'PSN', 'PSX', 'PTC', 'PUBM', 'PULS', 'PVH', 'PWR', 'PYPL', 'Q', 'QBTS',
    'QCLN', 'QCOM', 'QDEL', 'QLYS', 'QQQ', 'QS', 'QSR', 'QUAL', 'QYLD', 'R',
    'RACE', 'RARE', 'RBLX', 'RCAT', 'RCKT', 'RCKY', 'RCL', 'RCMT', 'RDDT', 'RDNT',
    'RDW', 'REG', 'REGN', 'REPL', 'REXR', 'RF', 'RGEN', 'RGLD', 'RGTI', 'RH',
    'RHI', 'RIG', 'RIO', 'RIOT', 'RITM', 'RIVN', 'RJF', 'RKLB', 'RKT', 'RLAY',
    'RLI', 'RMBS', 'RMD', 'RNG', 'RNR', 'ROK', 'ROKU', 'ROL', 'ROP', 'ROST',
    'RPD', 'RPM', 'RPRX', 'RRC', 'RSG', 'RTX', 'RUN', 'RUSHA', 'RXO', 'RXST',
    'RY', 'RYLD', 'RYN', 'SAIA', 'SAIC', 'SAM', 'SANM', 'SAP', 'SBLK', 'SBUX',
    'SCHA', 'SCHB', 'SCHD', 'SCHM', 'SCHP', 'SCHW', 'SCHX', 'SCI', 'SDGR', 'SEDG',
    'SEIC', 'SF', 'SFM', 'SG', 'SGHT', 'SGOL', 'SGOV', 'SHAK', 'SHC', 'SHEL',
    'SHLS', 'SHV', 'SHW', 'SHY', 'SID', 'SIGA', 'SIGI', 'SIL', 'SIRI', 'SITM',
    'SIVR', 'SIZE', 'SJM', 'SKY', 'SKYY', 'SLAB', 'SLB', 'SLG', 'SLV', 'SM',
    'SMCI', 'SMFG', 'SMH', 'SMTC', 'SNA', 'SNAP', 'SNDK', 'SNDL', 'SNDR', 'SNEX',
    'SNOW', 'SNPS', 'SO', 'SOFI', 'SOLV', 'SON', 'SONY', 'SOYB', 'SPG', 'SPGI',
    'SPHD', 'SPHQ', 'SPLV', 'SPOT', 'SPSC', 'SPXC', 'SPY', 'SR', 'SRAD', 'SRE',
    'SRLN', 'SRPT', 'SSD', 'STAA', 'STAG', 'STE', 'STGW', 'STLA', 'STLD', 'STM',
    'STNE', 'STNG', 'STOK', 'STRL', 'STT', 'STX', 'STZ', 'SU', 'SUPN', 'SVM',
    'SW', 'SWBI', 'SWK', 'SYF', 'SYK', 'SYNA', 'SYY', 'T', 'TAK', 'TAN',
    'TAP', 'TARS', 'TBBK', 'TBPH', 'TCBI', 'TCMD', 'TD', 'TDG', 'TDY', 'TEAM',
    'TECH', 'TECK', 'TEL', 'TEM', 'TENB', 'TER', 'TFC', 'TGT', 'TGTX', 'THC',
    'THD', 'TJX', 'TKO', 'TLT', 'TM', 'TMDX', 'TMO', 'TMUS', 'TNDM', 'TOL',
    'TOST', 'TOWN', 'TPL', 'TPR', 'TRGP', 'TRI', 'TRIP', 'TRMB', 'TRNS', 'TROW',
    'TRP', 'TRUP', 'TRV', 'TSCO', 'TSLA', 'TSM', 'TSN', 'TT', 'TTD', 'TTEC',
    'TTMI', 'TTWO', 'TUR', 'TWLO', 'TWST', 'TXG', 'TXN', 'TXT', 'TYL', 'U',
    'UAL', 'UBER', 'UDR', 'UEC', 'UHS', 'UL', 'ULTA', 'UMBF', 'UMC', 'UNG',
    'UNH', 'UNM', 'UNP', 'UPBD', 'UPS', 'UPST', 'UPWK', 'URA', 'URI', 'USB',
    'USCI', 'USFR', 'USMV', 'USO', 'UUP', 'V', 'VALE', 'VALN', 'VB', 'VBR',
    'VCEL', 'VCTR', 'VEA', 'VECO', 'VEEV', 'VERX', 'VGIT', 'VGLT', 'VGSH', 'VICI',
    'VIG', 'VIRT', 'VLO', 'VLTO', 'VLUE', 'VLY', 'VMC', 'VNM', 'VNQ', 'VONE',
    'VONG', 'VONV', 'VOO', 'VRNS', 'VRSK', 'VRSN', 'VRT', 'VRTX', 'VSEC', 'VST',
    'VT', 'VTI', 'VTOL', 'VTR', 'VTRS', 'VTV', 'VUG', 'VWO', 'VXF', 'VXUS',
    'VZ', 'WAB', 'WAL', 'WAT', 'WBD', 'WCC', 'WCN', 'WDAY', 'WDFC', 'WEAT',
    'WEC', 'WELL', 'WERN', 'WEX', 'WFC', 'WHR', 'WINA', 'WING', 'WIT', 'WIX',
    'WK', 'WM', 'WMB', 'WMS', 'WMT', 'WPM', 'WRB', 'WSM', 'WST', 'WTFC',
    'WTRG', 'WTW', 'WY', 'WYNN', 'XBI', 'XEL', 'XENE', 'XHB', 'XLC', 'XLE',
    'XLF', 'XLI', 'XLK', 'XLP', 'XLRE', 'XLU', 'XLV', 'XLY', 'XNCR', 'XOM',
    'XOP', 'XPEL', 'XPEV', 'XPO', 'XRAY', 'XRT', 'XYL', 'XYLD', 'XYZ', 'YUM',
    'YUMC', 'ZBH', 'ZBRA', 'ZETA', 'ZIM', 'ZION', 'ZM', 'ZROZ', 'ZS', 'ZTO',
    'ZTS', 'ZWS', 'ZYME',
]
    
print(len(STOCKS))
exit()
print("\n")

RED = "\033[31m"
GREEN = "\033[32m"
YELLOW = "\033[33m"
RESET = "\033[0m"

ALPACA_KEY = os.environ.get("ALPACA_KEY")
ALPACA_SECRET = os.environ.get("ALPACA_SECRET")

for stock in STOCKS[::]:
    command: str = 
f"""
curl --request GET \
     --url 'https://data.alpaca.markets/v2/stocks/bars?symbols={}&timeframe=1D&start=2016-01-01&end=2026-09-01&limit=10000&adjustment=all&feed=sip&sort=asc' \
     --header 'APCA-API-KEY-ID: {ALPACA_KEY}' \
     --header 'APCA-API-SECRET-KEY: {ALPACA_SECRET}' \
     --header 'accept: application/json' \
     > ./{stock}.json
"""
    
    while True:
        os.system(command)
        try:
            with open(f"./{stock}.json", "r") as f:
                data = json.load(f)
                assert (stock in data)
                if len(data[stock]) == 501:
                    print(GREEN + f"Succeeded for {stock}" + RESET)
                else:
                    os.system(f"rm ./{stock}.json")
                    print(RED + f"Skipping {stock}" + RESET)
                break
        except Exception:
            print(YELLOW + f"Retrying {stock}" + RESET)
            sleep(60)
            continue
            

