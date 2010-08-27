/*
 *  magicPointers.h
 *  Albert
 *
 *  Created by Matthias Melcher on 27.08.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

// Index / Name (v2 if avail, else v1) / ROM version / opt. comment

{ 0, "ROM_action_list", 1 },
{ 1, "ROM_addresseeSlip", 1 },
{ 2, "protoPeriodicAlarmEditor", 2 },
{ 3, "ROM_alarmQuerySpec", 1 },
{ 4, "ROM_alarmWakeup", 2, "Sound" },

#if 0
/*
      protoAnalogClock:			protoAnalogClock,			//  @5
      //  @6 ?
      //  @7 ?
      //  @8 ?
      //  @9 ?
      //protoPasswordSlip:		protoPasswordSlip,			// @10 protoNOS2 -- 2.1 only
      }
      
      ROM_analogClock:			ROM_analogClock,			//  @5
      ROM_asciiBreak:				ROM_asciiBreak,				//  @6
      ROM_asciiShift:				ROM_asciiShift,				//  @7
      ROM_assistFrames:			ROM_assistFrames,			//  @8
      ROM_backupSlip:				ROM_backupSlip,				//  @9
      ROM_badAddressMessage:		ROM_badAddressMessage,		// @10
      });
      
      // ===== 11-20
      
      call kAppendFrame with (partData,
      if kBuild20
      then { // 2.0
      ROM_checkBitmaps:			ROM_checkBitmaps,			// @11 [bitmap]
      ROM_priorityItems:			ROM_priorityItems,			// @12
      // @13 ?
      // @14 ?
      // @15 ?
      ROM_bootSound:				ROM_bootSound,				// @16 [sound]
      // @17 ?
      // @18 ?
      //protoRepeatDateDurationTextPicker:
      //protoRepeatDateDurationTextPicker,// @19 protNOS2
      //protoSenderPopup:			protoSenderPopup,			// @20 protNOS2
      }
      
      else { // 1.x
      ROM_bcBates:				ROM_bcBates,				// @11 [card style]
      ROM_bcCarlton:				ROM_bcCarlton,				// @12 [card style]
      ROM_bcHopkins:				ROM_bcHopkins,				// @13 [card style]
      ROM_bcRitz:					ROM_bcRitz,					// @14 [card style]
      ROM_blackbirdServiceName:	ROM_blackbirdServiceName,	// @15
      ROM_bootSound:				ROM_bootSound,				// @16 [sound]
      ROM_calculator:				ROM_calculator,				// @17
      ROM_calendar:				ROM_calendar,				// @18
      ROM_calendarList:			ROM_calendarList,			// @19
      ROM_calendarMailSlip:		ROM_calendarMailSlip,		// @20
      });
      
      // ===== 21-30
      Print('rom30);

call kAppendFrame with (partData,
                        if kBuild20
                        then { // 2.0
                               //protoStatusBarber:		protoStatusBarber,			// @21 protNOS2
                        ROM_calendarNotesName:		ROM_calendarNotesName,		// @22 "Calendar Notes"
                        ROM_calendarSoupName:		ROM_calendarSoupName,		// @23 "Calendar"
                                                                                        // @24 ?
                                                                                        // @25 ?
                        newtAreaCodeLine:			newtAreaCodeLine,			// @26
                                                                                                        // @27 ?
                        protoCanonicalCompass:		protoCanonicalCompass,		// @28
                                                                                        // @29 ?
                                                                                        // @30 ?
                        }
                        
                        else { // 1.x
                        ROM_calendarMeta:			ROM_calendarMeta,			// @21
                        ROM_calendarNotesName:		ROM_calendarNotesName,		// @22 "Calendar Notes"
                        ROM_calendarSoupName:		ROM_calendarSoupName,		// @23 "Calendar"
                        ROM_calendarStrings:		ROM_calendarStrings,		// @24 ["Every Day", "First", "Second",...]
                        ROM_calendarStuff:			ROM_calendarStuff,			// @25
                        ROM_calibrationQuery:		ROM_calibrationQuery,		// @26
                        ROM_callSlip:				ROM_callSlip,				// @27
                        ROM_canonicalCompass:		ROM_canonicalCompass,		// @28
                        ROM_canonicalContext:		ROM_canonicalContext,		// @29
                        ROM_canonicalCorrector:		ROM_canonicalCorrector,		// @30
                        });

// ===== 31-40

call kAppendFrame with (partData,
                        if kBuild20
                        then { // 2.0
                               // @31 ?
                               // @32 ?
                               // @33 ?
                               // @34 ?
                               // @35 ?
                               // @36 ?
                               // @37 ?
                               // @38 ?
                               // @39 ?
                               // @40 ?
                        }
                        
                        else { // 1.x
                        ROM_canonicalDataContext:	ROM_canonicalDataContext,	// @31
                        ROM_canonicalDate:			ROM_canonicalDate,			// @32
                        ROM_canonicalGroup:			ROM_canonicalGroup,			// @33
                        ROM_canonicalGroupee:		ROM_canonicalGroupee,		// @34
                        ROM_canonicalPopup:			ROM_canonicalPopup,			// @35
                        ROM_canonicalRect:			ROM_canonicalRect,			// @36
                        ROM_canonicalScrollee:		ROM_canonicalScrollee,		// @37
                        ROM_canonicalScroller:		ROM_canonicalScroller,		// @38
                        ROM_canonicalTable:			ROM_canonicalTable,			// @39
                        ROM_canonicalTextBlock:		ROM_canonicalTextBlock,		// @40
                        });

// ===== 41-50
Print('rom50);
      
      call kAppendFrame with (partData,
      if kBuild20
      then { // 2.0
      // @41 ?
      // @42 ?
      // @43 ?
      // @44 ?
      // @45 ?
      // @46 ?
      ROM_cardfileSoupName:		ROM_cardfileSoupName,		// @47 "Names"
      // @48 ?
      // @49 ?
      // @50 ?
      }
      
      else { // 1.x
      ROM_canonicalTitle:			ROM_canonicalTitle,			// @41
      ROM_capslockLight:			ROM_capslockLight,			// @42
      ROM_cardfile:				ROM_cardfile,				// @43
      ROM_cardfileIndices:		ROM_cardfileIndices,		// @44
      ROM_cardfileMeta:			ROM_cardfileMeta,			// @45
      ROM_cardfileQuerySpec:		ROM_cardfileQuerySpec,		// @46
      ROM_cardfileSoupName:		ROM_cardfileSoupName,		// @47 "Names"
      ROM_charsVersion:			ROM_charsVersion,			// @48
      ROM_checkingMessage:		ROM_checkingMessage,		// @49
      ROM_cities:					ROM_cities,					// @50
      });
      
      // ===== 51-60
      
      call kAppendFrame with (partData,
      if kBuild20
      then { // 2.0
      ROM_click:					ROM_click,					// @51 [sound]
      //protoFrameFormat:			protoFrameFormat,			// @52 protNOS2
      ROM_cloud1:					ROM_cloud1,					// @53 [bitmap]
      ROM_cloud2:					ROM_cloud2,					// @54 [bitmap]
      ROM_cloud3:					ROM_cloud3,					// @55 [bitmap]
      // @56 ?
      // @57 ?
      // @58 ?
      // @59 ?
      //protoCoverPageFormat: protoCoverPageFormatDeprecated, // @60 proterr1
      }
      
      else { // 1.x
      ROM_click:					ROM_click,					// @51 [sound]
      ROM_closeStoreRelatedForms: ROM_closeStoreRelatedForms,	// @52
      ROM_cloud1:					ROM_cloud1,					// @53 [bitmap] 1st cloud in erase animation
      ROM_cloud2:					ROM_cloud2,					// @54 [bitmap] 2nd cloud
      ROM_cloud3:					ROM_cloud3,					// @55 [bitmap] 3rd cloud
      ROM_connectMessage:			ROM_connectMessage,			// @56
      ROM_containerName:			ROM_containerName,			// @57
      ROM_copperfield:			ROM_copperfield,			// @58
      ROM_countries:				ROM_countries,				// @59
      //ROM_coverPageFormat:		ROM_coverPageFormat,		// @60 proterr1
      });
      
      // ===== 61-70
      Print('rom70);

call kAppendFrame with (partData,
                        if kBuild20
                        then { // 2.0
                               // @61 ?
                        ROM_crumple:				ROM_crumple,				// @62 [sound]
                        ROM_dataName:				ROM_dataName,				// @63
                                                                                                        // @64 ?
                                                                                                        // @65 ?
                        ROM_dateTimeStrSpecs:		ROM_dateTimeStrSpecs,		// @66
                                                                                        // @67 ?
                        ROM_dialTones:				ROM_dialTones,				// @68 [sound array]
                                                                                                        // @69 ?
                                                                                                        // @70 ?
                        }
                        
                        else { // 1.x
                        ROM_cribNote:				ROM_cribNote,				// @61
                                                                                                        //ROM_crumple:				ROM_crumple,				// @62 [sound]
                        ROM_dataName:				ROM_dataName,				// @63
                        ROM_dateIndices:			ROM_dateIndices,			// @64
                        ROM_dateQuerySpec:			ROM_dateQuerySpec,			// @65
                        ROM_dateTimeStrSpecs:		ROM_dateTimeStrSpecs,		// @66
                        ROM_dayView:				ROM_dayView,				// @67
                        ROM_dialTones:				ROM_dialTones,				// @68 [sound array] 12 phone sounds, e.g., PlaySound(@68[0])
                        ROM_dictionaries:			ROM_dictionaries,			// @69
                        ROM_dictionaryList:			ROM_dictionaryList,			// @70
                        });

// ===== 71-80

call kAppendFrame with (partData,
                        if kBuild20
                        then { // 2.0
                               // @71 ?
                               //protoNumberPicker:		protoNumberPicker,			// @72 protNOS2
                               //newtStatusBarNoClose:		newtStatusBarNoClose,		// @73 protNOS2
                               // @74 ?
                        protoSmartCluster:			protoSmartCluster,			// @75
                        ROM_drawerClose:			ROM_drawerClose,			// @76 [sound]
                        ROM_drawerOpen:				ROM_drawerOpen,				// @77 [sound]
                                                                                                        // @78 ?
                        ROM_errorTable:				ROM_errorTable,				// @79
                                                                                                        // @80 ?
                        }
                        
                        else { // 1.x
                        ROM_disconnectMessage:		ROM_disconnectMessage,		// @71
                        ROM_dockerChooser:			ROM_dockerChooser,			// @72
                        ROM_dockerRecentChooser:	ROM_dockerRecentChooser,	// @73
                        ROM_dockingMsg:				ROM_dockingMsg,				// @74
                        ROM_downloadingMessage:		ROM_downloadingMessage,		// @75
                        ROM_drawerClose:			ROM_drawerClose,			// @76 [sound]
                        ROM_drawerOpen:				ROM_drawerOpen,				// @77 [sound]
                        ROM_edgeDrawer:				ROM_edgeDrawer,				// @78
                        ROM_errorTable:				ROM_errorTable,				// @79
                        ROM_espyFont:				ROM_espyFont,				// @80 [font]
                        });

// ===== 81-86

call kAppendFrame with (partData,
                        if kBuild20
                        then { // 2.0
                               // @81 ?
                               // @82 ?
                               // @83 ?
                               // @84 ?
                        ROM_flip:					ROM_flip,					// @85 [sound]
                                                                                                                        // @86 ?
                        }
                        else {
                        ROM_faxDriver:				ROM_faxDriver,				// @81
                        ROM_faxHeader:				ROM_faxHeader,				// @82
                        ROM_faxSlip:				ROM_faxSlip,				// @83
                        ROM_filingSlip:				ROM_filingSlip,				// @84
                        ROM_flip:					ROM_flip,					// @85 [sound]
                        ROM_folderListQuery:		ROM_folderListQuery,		// @86
                        });

// ===== 87-102
Print('rom90);
      
      call kAppendFrame with (partData,
      { // 1.x and 2.0
      ROM_fontSystem10:			ROM_fontSystem10,			// @87 [font] 0x00002800 {family: 'espy, face: 0, size: 10}
      ROM_fontSystem10Bold:		ROM_fontSystem10Bold,		// @88 [font] 0x00102800 {family: 'espy, face: 1, size: 10}
      ROM_fontSystem10Underline:	ROM_fontSystem10Underline,	// @89 [font] 0x00402800 {family: 'espy, face: 4, size: 10}
      ROM_fontSystem12:			ROM_fontSystem12,			// @90 [font] 0x00003000 {family: 'espy, face: 0, size: 12}
      ROM_fontSystem12Bold:		ROM_fontSystem12Bold,		// @91 [font] 0x00103000 {family: 'espy, face: 1, size: 12}
      ROM_fontSystem12Underline:	ROM_fontSystem12Underline,	// @92 [font] 0x00403000 {family: 'espy, face: 4, size: 12}
      ROM_fontSystem14:			ROM_fontSystem14,			// @93 [font] 0x00003800 {family: 'espy, face: 0, size: 14}
      ROM_fontSystem14Bold:		ROM_fontSystem14Bold,		// @94 [font] 0x00103800 {family: 'espy, face: 1, size: 14}
      ROM_fontSystem14Underline:	ROM_fontSystem14Underline,	// @95 [font] 0x00403800 {family: 'espy, face: 4, size: 14}
      ROM_fontSystem18:			ROM_fontSystem18,			// @96 [font] 0x00004800 {family: 'espy, face: 0, size: 18}
      ROM_fontSystem18Bold:		ROM_fontSystem18Bold,		// @97 [font] 0x00104800 {family: 'espy, face: 1, size: 18}
      ROM_fontSystem18Underline:	ROM_fontSystem18Underline,	// @98 [font] 0x00404800 {family: 'espy, face: 4, size: 18}
      ROM_fontSystem9:			ROM_fontSystem9,			// @99 [font] 0x00002400 {family: 'espy, face: 0, size:  9}   
      ROM_fontSystem9Bold:		ROM_fontSystem9Bold,		//@100 [font] 0x00102400 {family: 'espy, face: 1, size:  9}  
      ROM_fontSystem9Underline:	ROM_fontSystem9Underline,	//@101 [font] 0x00402400 {family: 'espy, face: 4, size:  9}  
      ROM_funBeep:				ROM_funBeep,				//@102 [sound]
      });

// ===== 103-110
Print('rom110);
      
      call kAppendFrame with (partData,
      if kBuild20
      then {													//@103 ?
      //@104 ?
      //@105 ?
      //@106 ?
      //newtPrefsView:			newtPrefsView,				//@107 protNOS2
      protoYearPicker:			protoYearPicker,			//@108
      //@109 ?
      ROM_hiliteSound:			ROM_hiliteSound,			//@110 [sound]
      }
      else {
      ROM_genericPrinterError:	ROM_genericPrinterError,	//@103
      ROM_genevaFont:				ROM_genevaFont,				//@104 [font]
      ROM_gtPens:					ROM_gtPens,					//@105 [bitmap array] 6 pens in diff orientations
      ROM_gtScenes:				ROM_gtScenes,				//@106
      ROM_handwritingPracticePreferencesForm:					//@107
      ROM_handwritingPracticePreferencesForm,
      ROM_helpBook:				ROM_helpBook,				//@108
      ROM_helveticaFont:			ROM_helveticaFont,			//@109 [font]
      ROM_hiliteSound:			ROM_hiliteSound,			//@110 [sound]
      });
      
      
      // ===== 111-120
      
      call kAppendFrame with (partData,
      if kBuild20
      then { // 2.0
      ROM_inboxsoupname:			ROM_inboxsoupname,			//@111 "Inbox"
      //@112-120 ?
      }
      else { // 1.x
      ROM_inboxsoupname:			ROM_inboxsoupname,			//@111 "Inbox"
      ROM_initialInheritanceFrame:ROM_initialInheritanceFrame,//@112
      ROM_initScripts:			ROM_initScripts,			//@113
      ROM_ioIndices:				ROM_ioIndices,				//@114
      ROM_letters:				ROM_letters,				//@115
      ROM_letterWeightQuery:		ROM_letterWeightQuery,		//@116
      ROM_loadCalibration:		ROM_loadCalibration,		//@117
      ROM_loadConstants:			ROM_loadConstants,			//@118
      ROM_loadLetterWeights:		ROM_loadLetterWeights,		//@119
      ROM_mailEditor:				ROM_mailEditor,				//@120
      });
      
      // ===== 121-130
      Print('rom130);

call kAppendFrame with (partData,
                        if kBuild20
                        then { // 2.0
                               //@121 ?
                               //@122 ?
                               //protoRepeatPicker:		protoRepeatPicker,			//@123 protNOS2
                               //newtInfoButton:			newtInfoButton,				//@124 protNOS2
                               //@125 ?
                               //@126 ?
                               //@127 ?
                        protoTimeButton:			protoTimeButton,			//@128
                        ROM_metaSoupName:			ROM_metaSoupName,			//@129 "Directory"
                                                                                                        //@130 ?
                        }
                        else { // 1.x
                        ROM_mailRegister:			ROM_mailRegister,			//@121
                        ROM_mailSlip:				ROM_mailSlip,				//@122
                        ROM_mainToDo:				ROM_mainToDo,				//@123
                        ROM_makeCardfileSoup:		ROM_makeCardfileSoup,		//@124
                        ROM_meeting:				ROM_meeting,				//@125
                        ROM_meetingName:			ROM_meetingName,			//@126 "meeting"
                        ROM_messageNotification:	ROM_messageNotification,	//@127
                        ROM_metaArray:				ROM_metaArray,				//@128
                        ROM_metaSoupName:			ROM_metaSoupName,			//@129 "Directory"
                        ROM_netChooser:				ROM_netChooser,				//@130
                        });

// ===== 131-140

call kAppendFrame with (partData,
                        if kBuild20
                        then { // 2.0											//@131 ?
                               //protoDragger:				protoDragger,				//@132 protNOS2
                               //@133 ?
                        protoNotesContent:			protoNotesContent,			//@134
                                                                                                        //@135 ?
                                                                                                        //@136 ?
                                                                                                        //@137 ?
                                                                                                        //@138 ?
                        ROM_outboxSoupName:			ROM_outboxSoupName,			//@139 "Outbox"
                                                                                                        //@140 ?
                        }
                        else { // 1.x
                        ROM_newYorkFont:			ROM_newYorkFont,			//@131 [font]
                        ROM_noMessage:				ROM_noMessage,				//@132
                        ROM_notePaper:				ROM_notePaper,				//@133
                        ROM_notesSlip:				ROM_notesSlip,				//@134
                        ROM_notification:			ROM_notification,			//@135
                        ROM_notifyIcon:				ROM_notifyIcon,				//@136 [bitmap]
                        ROM_onlineMessages:			ROM_onlineMessages,			//@137
                        ROM_onlineServices:			ROM_onlineServices,			//@138
                        ROM_outboxSoupName:			ROM_outboxSoupName,			//@139 "Outbox"
                        ROM_pageFooter:				ROM_pageFooter,				//@140
                        });

// ===== 141-150
Print('rom150);
      
      call kAppendFrame with (partData,
      if kBuild20
      then { // 2.0											//@141 ?
      //@142 ?
      //@143 ?
      ROM_paperRollSoupName:		ROM_paperRollSoupName,		//@144 "Notes"
      //@145 ?
      newtCountrySymbolFilter:	newtCountrySymbolFilter,	//@146
      //protoTimePopup:			protoTimePopup,				//@147 protNOS2
      ROM_phoneText:				ROM_phoneText,				//@148
      //@149 ?
      ROM_plinkBeep:				ROM_plinkBeep,				//@150 [sound]
      }
      
      else { // 1.x
      ROM_pagePreviewForm:		ROM_pagePreviewForm,		//@141
      ROM_paperRollIndices:		ROM_paperRollIndices,		//@142
      ROM_paperRollMeta:			ROM_paperRollMeta,			//@143
      ROM_paperRollSoupName:		ROM_paperRollSoupName,		//@144 "Notes"
      ROM_paragraphCodeBook2:		ROM_paragraphCodeBook2,		//@145
      ROM_phoneClasses:			ROM_phoneClasses,			//@146
      ROM_phoneLabels:			ROM_phoneLabels,			//@147
      ROM_phoneText:				ROM_phoneText,				//@148
      ROM_phoneView:				ROM_phoneView,				//@149
      ROM_plinkBeep:				ROM_plinkBeep,				//@150 [sound]
      });
      
      // ===== 151-160
      
      call kAppendFrame with (partData,
      if kBuild20
      then { // 2.0
      //@151 ?
      //newtAboutView:			newtAboutView,				//@152 protNOS2
      //protoPrinterChooserButton:protoPrinterChooserButton,	//@153 protNOS2
      //@154 ?
      //@155 ?
      //@156 ?
      //protoApp:					protoApp,					//@157 proterr1
      //@158 ?
      //@159 ?
      //protoBorder:				protoBorder,				//@160 proterr1
      }
      
      else { // 1.x
      ROM_preparingMessage:		ROM_preparingMessage,		//@151
      ROM_primaryNameView:		ROM_primaryNameView,		//@152
      ROM_printerChooserButton:	ROM_printerChooserButton,	//@153
      ROM_printerSerialPicker:	ROM_printerSerialPicker,	//@154
      ROM_printPageMessage:		ROM_printPageMessage,		//@155
      ROM_printSlip:				ROM_printSlip,				//@156
      //protoApp:					protoApp,					//@157 proterr1
      protoBook:					protoBook,					//@158
      protoBookmark:				protoBookmark,				//@159
      //protoBorder:				protoBorder,				//@160 proterr1
      });
      
      // ===== 161-170
      Print('rom170);

call kAppendFrame with (partData,
                        if kBuild20
                        then { // 2.0											//@161 ?
                               //newtClockFolderTab:		newtClockFolderTab,			//@162 protNOS2
                               //protoLargeClosebox:		protoLargeClosebox,			//@163 proterr1
                               //protoCheckbox:			protoCheckbox,				//@164 proterr1
                        protoCheckboxIcon:			protoCheckboxIcon,			//@165
                                                                                                        //protoClosebox:			protoClosebox,				//@166 proterr1
                                                                                                        //@167 ?
                                                                                                        //@168 ?
                                                                                                        //@169 ?
                                                                                                        //protoDateExpando:			protoDateExpandoDeprecated,	//@170 proterr1
                          
                        }
                        else { // 1.x
                        protoBottomTitle:			protoBottomTitle,			//@161
                        protoCalendarOverview:		protoCalendarOverview,		//@162
                        protoCancelButton:			protoCancelButton,			//@163 (use protoLargeCloseBox)
                                                                                                        //protoLargeClosebox:		protoLargeClosebox,			//@163 proterr1
                                                                                                        //protoCheckbox:			protoCheckbox,				//@164 proterr1
                        protoCheckboxIcon:			protoCheckboxIcon,			//@165 proterr1
                                                                                                        //protoClosebox:			protoClosebox,				//@166 proterr1
                        protoCorrectContext:		protoCorrectContext,		//@167
                        protoCorrector:				protoCorrector,				//@168
                        protoCursiveCheckbox:		protoCursiveCheckbox,		//@169
                                                                                        //protoDateExpando:			protoDateExpando,			//@170 proterr1
                        });

// ===== 171-180

call kAppendFrame with (partData,
                        if kBuild20
                        then { // 2.0											//@171 ?
                               //protoDivider:				protoDivider,				//@172 proterr1
                               //protoDrawer:				protoDrawer,				//@173 proterr1
                               //protoEndpoint:			protoEndpoint,				//@174 CommCnst
                               //protoExpandoShell:		protoExpandoShellDeprecated,//@175 proterr1
                               //protoFilingButton:		protoFilingButton,			//@176 proterr1
                               //@177 ?
                               //@178 ?
                               //protoFloater:				protoFloater,				//@179 proterr1
                               //protoFloatNGo:			protoFloatNGo,				//@180 proterr1
                        }
                        
                        else { // 1.x
                        protoDictionary:			protoDictionary,			//@171
                                                                                                        //protoDivider:				protoDivider,				//@172 proterr1
                                                                                                        //protoDrawer:				protoDrawer,				//@173 proterr1
                                                                                                        //protoEndpoint:			protoEndpoint,				//@174 CommCnst
                                                                                                        //protoExpandoShell:		protoExpandoShell,			//@175 proterr1
                                                                                                        //protoFilingButton:		protoFilingButton,			//@176 proterr1
                        protoFindCategory:			protoFindCategory,			//@177
                        protoFindItem:				protoFindItem,				//@178
                                                                                                        //protoFloater:				protoFloater,				//@179 proterr1
                                                                                                        //protoFloatNGo:			protoFloatNGo,				//@180 proterr1
                        });

// ===== 181-190
Print('rom190);
      
      call kAppendFrame with (partData,
      if kBuild20
      then { // 2.0											//@181 ?
      //protoGauge:				protoGauge,					//@182 proterr1
      //protoGlance:				protoGlance,				//@183 proterr1
      //@184 ?
      //protoInputLine:			protoInputLine,				//@185 proterr1
      //@186 ?
      //protoKeyboard:			protoKeyboard,				//@187 proterr1
      //protoKeypad:				protoKeypad,				//@188 proterr1
      //protoLabelInputLine:		protoLabelInputLine,		//@189 proterr1
      //protoLabelPicker:			protoLabelPicker,			//@190 proterr1
      }
      
      else { // 1.x
      protoFormulaOptionCheckbox:	protoFormulaOptionCheckbox,	//@181
      //protoGauge:				protoGauge,					//@182 proterr1
      //protoGlance:				protoGlance,				//@183 proterr1
      protoHiliteButton:			protoHiliteButton,			//@184
      //protoInputLine:			protoInputLine,				//@185 proterr1
      protoIOCategory:			protoIOCategory,			//@186
      //protoKeyboard:			protoKeyboard,				//@187 proterr1
      //protoKeypad:				protoKeypad,				//@188 proterr1
      //protoLabelInputLine:		protoLabelInputLine,		//@189 proterr1
      //protoLabelPicker:			protoLabelPicker,			//@190 proterr1
      });
      
      // ===== 191-200
      
      call kAppendFrame with (partData,
      if kBuild20
      then { // 2.0
      //protoOverview:			protoOverview,				//@191 proterr1
      //@192 ?
      //@193 ?
      //protoPhoneExpando:		protoPhoneExpandoDeprecated,//@194 proterr1
      //protoPicker:				protoPicker,				//@195 proterr1
      //protoPictIndexer:			protoPictIndexer,			//@196 proterr1
      //protoPictRadioButton:		protoPictRadioButton,		//@197 proterr1
      //protoPictureButton:		protoPictureButton,			//@198 proterr1
      //@199 ?
      //protoPrintFormat:			protoPrintFormat,			//@200 proterr1
      }
      
      else { // 1.x
      //protoOverview:			protoOverview,				//@191 proterr1
      protoOverviews:				protoOverviews,				//@192
      protoParagraph:				protoParagraph,				//@193
      //protoPhoneExpando:		protoPhoneExpando,			//@194 proterr1
      //protoPicker:				protoPicker,				//@195 proterr1
      //protoPictIndexer:			protoPictIndexer,			//@196 proterr1
      //protoPictRadioButton:		protoPictRadioButton,		//@197 proterr1
      //protoPictureButton:		protoPictureButton,			//@198 proterr1
      protoPolygon:				protoPolygon,				//@199
      //protoPrintFormat:			protoPrintFormat,			//@200 proterr1
      });
      
      // ===== 201-210
      Print('rom210);

call kAppendFrame with (partData,
                        if kBuild20
                        then { // 2.0											//@201 ?
                               //protoRadioButton:			protoRadioButton,			//@202 proterr1
                               //protoRadioCluster:		protoRadioCluster,			//@203 proterr1
                               //protoRCheckbox:			protoRCheckbox,				//@204 proterr1
                               //@205 ?
                               //protoRoll:				protoRoll,					//@206 proterr1
                               //protoRollBrowser:			protoRollBrowser,			//@207 proterr1
                               //protoRollItem:			protoRollItem,				//@208 proterr1
                               //protoActionButton:		protoActionButton,			//@209 proterr1
                               //protoSetClock:			protoSetClock,				//@210 proterr1
                        }
                        
                        else { // 1.x
                        protoPrintPage:				protoPrintPage,				//@201
                                                                                                        //protoRadioButton:			protoRadioButton,			//@202 proterr1
                                                                                                        //protoRadioCluster:		protoRadioCluster,			//@203 proterr1
                                                                                                        //protoRCheckbox:			protoRCheckbox,				//@204 proterr1
                        protoRecognitionCheckbox:	protoRecognitionCheckbox,	//@205
                                                                                        //protoRoll:				protoRoll,					//@206 proterr1
                                                                                        //protoRollBrowser:			protoRollBrowser,			//@207 proterr1
                                                                                        //protoRollItem:			protoRollItem,				//@208 proterr1
                        protoRoutingSlip:			protoRoutingSlip,			//@209 (use protoActionButton)
                                                                                                        //protoActionButton:		protoActionButton,			//@209 proterr1
                                                                                                        //protoSetClock:			protoSetClock,				//@210 proterr1
                        });

// ===== 211-220

call kAppendFrame with (partData,
                        if kBuild20
                        then { // 2.0
                               //protoFolderTab:			protoFolderTab,				//@211 proterr1
                               //protoSlider:				protoSlider,				//@212 proterr1
                               //@213 ?
                               //@214 ?
                               //@215 ?
                               //@216 ?
                               //@217 ?
                               //protoStaticText:			protoStaticText,			//@218 proterr1
                               //protoStatus:				protoStatus,				//@219 proterr1
                               //protoStatusBar:			protoStatusBar,				//@220 proterr1
                        }
                        
                        else { // 1.x
                        protoShowBar:				protoShowBar,				//@211 (use protoFolderTab)
                                                                                                        //protoFolderTab:			protoFolderTab,				//@211 proterr1
                                                                                                        //protoSlider:				protoSlider,				//@212 proterr1
                        protoSmartAddressLine:		protoSmartAddressLine,		//@213
                        protoSmartDateLine:			protoSmartDateLine,			//@214
                        protoSmartEmailLine:		protoSmartEmailLine,		//@215
                        protoSmartNameLine:			protoSmartNameLine,			//@216
                        protoSmartPhoneLine:		protoSmartPhoneLine,		//@217
                                                                                        //protoStaticText:			protoStaticText,			//@218 proterr1
                                                                                        //protoStatus:				protoStatus,				//@219 proterr1
                                                                                        //protoStatusBar:			protoStatusBar,				//@220 proterr1
                        });

// ===== 221-230
Print('rom230);
      
      call kAppendFrame with (partData,
      if kBuild20
      then { // 2.0											//@221 ?
      //@222 ? 
      //protoTable:				protoTable,					//@223 proterr1
      //protoTableDef:			protoTableDef,				//@224 proterr1
      //protoTableEntry:			protoTableEntry,			//@225 proterr1
      //protoTextButton:			protoTextButton,			//@226 proterr1
      //protoTextExpando:			protoTextExpandoDeprecated,	//@227 proterr1
      //protoTextList:			protoTextList,				//@228 proterr1
      //protoTitle:				protoTitle,					//@229 proterr1
      //@230 ?
      }
      
      else { // 1.x
      protoStoryCard:				protoStoryCard,				//@221
      protoStrokesItem:			protoStrokesItem,			//@222
      //protoTable:				protoTable,					//@223 proterr1
      //protoTableDef:			protoTableDef,				//@224 proterr1
      //protoTableEntry:			protoTableEntry,			//@225 proterr1
      //protoTextButton:			protoTextButton,			//@226 proterr1
      //protoTextExpando:			protoTextExpando,			//@227 proterr1
      //protoTextList:			protoTextList,				//@228 proterr1
      //protoTitle:				protoTitle,					//@229 proterr1
      protoZonesTable:			protoZonesTable,			//@230
      });
      
      // ===== 231-240
      
      call kAppendFrame with (partData,
      if kBuild20
      then { // 2.0
      //protoPeopleDataDef:		protoPeopleDataDef,			//@231 protNOS2
      //@232 ?
      //@233 ?
      //protoRecToggle:			protoRecToggle,				//@234 proterr1
      //@235 ?
      protoStdClosing:			protoStdClosing,			//@236
      ROM_soupFinder:				ROM_soupFinder,				//@237
      //@238 ?
      ROM_repeatMeetingName:		ROM_repeatMeetingName,		//@239 "Repeat Meetings"
      //protoNameRefDataDef:		protoNameRefDataDef,		//@240 protNOS2
      }
      
      else { // 1.x
      ROM_ramArraySetup:			ROM_ramArraySetup,			//@231
      ROM_ramFrameSetup:			ROM_ramFrameSetup,			//@232
      ROM_readingMessage:			ROM_readingMessage,			//@233
      ROM_recToggle:				ROM_recToggle,				//@234 (use protoRecToggle)
      //protoRecToggle:			protoRecToggle,				//@234 proterr1
      ROM_remindSlip:				ROM_remindSlip,				//@235
      ROM_removeForm:				ROM_removeForm,				//@236
      ROM_removeOldMeetingsForm:	ROM_removeOldMeetingsForm,	//@237
      ROM_repeatIndices:			ROM_repeatIndices,			//@238
      ROM_repeatMeetingName:		ROM_repeatMeetingName,		//@239 "Repeat Meetings"
      ROM_repeatMeetingsMeta:		ROM_repeatMeetingsMeta,		//@240
      });
      
      // ===== 241-250
      Print('rom250);

call kAppendFrame with (partData,
                        if kBuild20
                        then { // 2.0
                        ROM_nameRefValidationFrame: ROM_nameRefValidationFrame,	//@241
                        ROM_repeatNotesName:		ROM_repeatNotesName,		//@242 "Repeat Notes"
                                                                                        //@243 ?
                                                                                        //@244 ?
                                                                                        //@245 ?
                                                                                        //@246 ?
                                                                                        //@247 ?
                                                                                        //@248 ?
                                                                                        //@249 ?
                                                                                        //@250 ?
                        }
                        
                        else { // 1.x
                        ROM_repeatNotesMeta:		ROM_repeatNotesMeta,		//@241
                        ROM_repeatNotesName:		ROM_repeatNotesName,		//@242 "Repeat Notes"
                        ROM_repeatQuerySpec:		ROM_repeatQuerySpec,		//@243
                        ROM_reviewDict:				ROM_reviewDict,				//@244
                        ROM_romAvailablePrinters:	ROM_romAvailablePrinters,	//@245
                        ROM_romInternational:		ROM_romInternational,		//@246
                        ROM_romMetaData:			ROM_romMetaData,			//@247
                        ROM_romPhrasalLexicon:		ROM_romPhrasalLexicon,		//@248
                        ROM_salutationSuffix:		ROM_salutationSuffix,		//@249
                        ROM_saveCalibration:		ROM_saveCalibration,		//@250
                        });

// ===== 251-260

call kAppendFrame with (partData,
                        if kBuild20
                        then { // 2.0
                        ROM_citySoupName:			ROM_citySoupName,			//@251 "Cities"
                                                                                                        //@252 ?
                                                                                                        //@253 ?
                                                                                                        //@254 ?
                                                                                                        //@255 ?
                                                                                                        //@256 ?
                        ROM_radioOnBitmap:			ROM_radioOnBitmap,			//@257 [bitmap]
                                                                                                        //@258 ?
                                                                                                        //protoAddressPicker:		protoAddressPicker,			//@259 protNOS2
                                                                                                        //protoRoutingFormat:		protoRoutingFormat,			//@260 protNOS2
                        }
                        
                        else { // 1.x
                        ROM_savedFrames:			ROM_savedFrames,			//@251
                        ROM_saveLetterWeights:		ROM_saveLetterWeights,		//@252
                        ROM_scheduleSlip:			ROM_scheduleSlip,			//@253
                        ROM_scheduleView:			ROM_scheduleView,			//@254
                        ROM_searchPrefix:			ROM_searchPrefix,			//@255
                        ROM_searchSuffix:			ROM_searchSuffix,			//@256
                        ROM_secondaryNameView:		ROM_secondaryNameView,		//@257
                        ROM_sendMeeting:			ROM_sendMeeting,			//@258
                        ROM_sendMessage:			ROM_sendMessage,			//@259
                        ROM_setupNewForm:			ROM_setupNewForm,			//@260
                        });

// ===== 261-270
Print('rom270);
      
      call kAppendFrame with (partData,
      if kBuild20
      then { // 2.0
      ROM_shapeName:				ROM_shapeName,				//@261
      ROM_simpleBeep:				ROM_simpleBeep,				//@262 [sound]
      ROM_soundOff:				ROM_soundOff,				//@263 [sound frame]
      //@264 ?
      //@265 ?
      //@266 ?
      //@267 ?
      //@268 ?
      //@269 ?
      //@270 ?
      }
      
      else { // 1.x
      ROM_shapeName:				ROM_shapeName,				//@261
      ROM_simpleBeep:				ROM_simpleBeep,				//@262 [sound]
      ROM_soundOff:				ROM_soundOff,				//@263 [sound frame] 4 sounds (simpleBeep, wakeupBeep, plinkBeep, funBeep)
      ROM_standardStyles:			ROM_standardStyles,			//@264 [font array] = ROM_fontsystem9,10,12,14,18,9b,10b,12b,14b,18b,9i,10i,12i,14i,18i
      ROM_starterClipboard:		ROM_starterClipboard,		//@265
      ROM_starterInk:				ROM_starterInk,				//@266
      ROM_starterParagraph:		ROM_starterParagraph,		//@267
      ROM_starterPolygon:			ROM_starterPolygon,			//@268
      ROM_stdForms:				ROM_stdForms,				//@269
      ROM_stopWordList2:			ROM_stopWordList2,			//@270
      });
      
      // ===== 271-280
      
      call kAppendFrame with (partData,
      if kBuild20
      then { // 2.0											//@271 ?
      //@272 ?
      //@273 ?
      //@274 ?
      //@275 ?
      ROM_systemSoupName:			ROM_systemSoupName,			//@276 "System"
      //@277 ?
      //@278 ?
      //protoRepeatView:			protoRepeatView,			//@279
      //protoAnalogTimePopup:		protoAnalogTimePopup,		//@280
      }
      
      else { // 1.x
      ROM_stopWordList3:			ROM_stopWordList3,			//@271
      ROM_symbolFont:				ROM_symbolFont,				//@272 [font]
      ROM_systemFont:				ROM_systemFont,				//@273 'espy
      ROM_systemPSFont:			ROM_systemPSFont,			//@274 'helvetica
      ROM_systemSoupIndexes:		ROM_systemSoupIndexes,		//@275
      ROM_systemSoupName:			ROM_systemSoupName,			//@276 "System"
      ROM_systemSymbolFont:		ROM_systemSymbolFont,		//@277 [font]
      ROM_timesRomanFont:			ROM_timesRomanFont,			//@278 [font]
      ROM_toDoItem:				ROM_toDoItem,				//@279
      ROM_toDoMeta:				ROM_toDoMeta,				//@280
      });


// ===== 281-290
Print('rom290);
      
      call kAppendFrame with (partData,
      if kBuild20
      then { // 2.0											//@281 ?
      ROM_toDoSoupName:			ROM_toDoSoupName,			//@282 "To do"
      //@283 ?
      //protoTimeIntervalPopup:	protoTimeIntervalPopup,		//@284 protNOS2
      //@285 ?
      //@286 ?
      //@287 ?
      //protoDateNTimePopup:		protoDateNTimePopup,		//@288 protNOS2
      ROM_wakeUpBeep:				ROM_wakeUpBeep,				//@289 [sound]
      //@290 ?
      }
      
      else { // 1.x
      ROM_toDoName:				ROM_toDoName,				//@281 "todo"
      ROM_toDoSoupName:			ROM_toDoSoupName,			//@282 "To do"
      ROM_unicode:				ROM_unicode,				//@283
      ROM_uploadingMessage:		ROM_uploadingMessage,		//@284 "Sending attachmentŠ"
      ROM_userConfiguration:		ROM_userConfiguration,		//@285
      ROM_userDictquery:			ROM_userDictquery,			//@286
      ROM_viewRoot:				ROM_viewRoot,				//@287
      ROM_waitingMessage:			ROM_waitingMessage,			//@288 "Ready to Send"
      ROM_wakeUpBeep:				ROM_wakeUpBeep,				//@289 [sound]
      ROM_wizardImport:			ROM_wizardImport,			//@290
      });
      
      // ===== 291-300
      
      call kAppendFrame with (partData,
      if kBuild20
      then { // 2.0
      ROM_routeDeleteIcon:		ROM_routeDeleteIcon,		//@291 [bitmap]
      ROM_routeDuplicateIcon:		ROM_routeDuplicateIcon,		//@292 [bitmap]
      //@293 ?
      newtAreaCodePhoneLine:		newtAreaCodePhoneLine,		//@294
      ROM_countrySoupName:		ROM_countrySoupName,		//@295 "Countries"
      //@296 ?
      ROM_radioOffBitmap:			ROM_radioOffBitmap,			//@297 [bitmap]
      //@298?
      ROM_tickSound:				ROM_tickSound,				//@299 [sound]
      ROM_tockSound:				ROM_tockSound,				//@300 [sound]
      }
      
      else { // 1.x
      ROM_wizBadTitle:			ROM_wizBadTitle,			//@291
      ROM_wizDatesTitle:			ROM_wizDatesTitle,			//@292
      ROM_wizFinished:			ROM_wizFinished,			//@293
      ROM_wizImporting:			ROM_wizImporting,			//@294
      ROM_wizNamesTitle:			ROM_wizNamesTitle,			//@295
      ROM_wizNotesTitle:			ROM_wizNotesTitle,			//@296
      ROM_wizReceiving:			ROM_wizReceiving,			//@297
      ROM_worldClock:				ROM_worldClock,				//@298
      ROM_zapConfirmMsg:			ROM_zapConfirmMsg,			//@299
      ROM_zapNoMsg:				ROM_zapNoMsg,				//@300
      });
      
      // ===== 301-310
      Print('rom310);

call kAppendFrame with (partData,
                        if kBuild20
                        then { // 2.0
                        ROM_cuckooSound:			ROM_cuckooSound,			//@301 [sound]
                        ROM_ratchetSound:			ROM_ratchetSound,			//@302 [sound]
                        ROM_addSound:				ROM_addSound,				//@303 [sound]
                        ROM_removeSound:			ROM_removeSound,			//@304 [sound]
                        protoLetterFormat:			protoLetterFormat,			//@305
                                                                                                        //@306 ?
                                                                                                        //@307 ?
                                                                                                        //@308 ?
                                                                                                        //newtEntryPageHeader:		newtEntryPageHeader,		//@309 [protNOS2
                                                                                                        //@310 ?
                        }
                        
                        else { // 1.x
                        ROM_zapReceiveConfirm:		ROM_zapReceiveConfirm,		//@301
                        ROM_zapRecvCancelMsg:		ROM_zapRecvCancelMsg,		//@302
                        ROM_zapRecvConnectMsg:		ROM_zapRecvConnectMsg,		//@303
                        ROM_zapRecvDoneMsg:			ROM_zapRecvDoneMsg,			//@304
                        ROM_zapRecvMsg:				ROM_zapRecvMsg,				//@305
                        ROM_zapSendConnectMsg:		ROM_zapSendConnectMsg,		//@306
                        ROM_zapSendDoneMsg:			ROM_zapSendDoneMsg,			//@307
                        ROM_zapSendMsg:				ROM_zapSendMsg,				//@308
                        ROM_zapSlip:				ROM_zapSlip,				//@309
                        ROM_zoneChooser:			ROM_zoneChooser,			//@310
                        });

// ===== 311-320

call kAppendFrame with (partData,
                        if kBuild20
                        then { // 2.0
                        ROM_cancelBitmap:			ROM_cancelBitmap,			//@311 [bitmap] X
                                                                                                        //@312 ROM_routingBitmap?
                        ROM_plunk:					ROM_plunk,					//@313 [sound]
                        ROM_poof:					ROM_poof,					//@314 [sound]
                                                                                                                        //@315 ?
                                                                                                                        //protoLabeledBatteryGauge:	protoLabeledBatteryGauge,	//@316 proterr1
                                                                                                                        //protoDatePopup:			protoDatePopup,				//@317 protNOS2
                                                                                                                        //@318 ?
                                                                                                                        //@319 ?
                                                                                                                        //@320 ROM_bootLogoBitmap?
                        }
                        
                        else { // 1.x
                        ROM_cancelBitmap:			ROM_cancelBitmap,			//@311 [bitmap] X
                        ROM_routingBitmap:			ROM_routingBitmap,			//@312 [bitmap] envelope icon (protoActionButton.icon)
                        ROM_plunk:					ROM_plunk,					//@313 [sound]
                        ROM_poof:					ROM_poof,					//@314 [sound]
                        protoBatteryGauge:			protoBatteryGauge,			//@315
                                                                                                        //protoLabeledBatteryGauge:	protoLabeledBatteryGauge,	//@316 proterr1
                        ROM_toDoOverview:			ROM_toDoOverview,			//@317
                        protoInCategory:			protoInCategory,			//@318
                        protoOutCategory:			protoOutCategory,			//@319
                        ROM_bootLogoBitmap:			ROM_bootLogoBitmap,			//@320 [pict] v. large lightbulb (boot screen)
                        });

// ===== 321-329
Print('rom330);
      
      call kAppendFrame with (partData,
      { // 1.x and 2.0
      ROM_worldMapBitmap:			ROM_worldMapBitmap,			//@321 [bitmap] world map (time zones)
      ROM_phoneBitmap:			ROM_phoneBitmap,			//@322 [bitmap] phone
      ROM_upBitmap:				ROM_upBitmap,				//@323 [bitmap] big up arrow 	
      ROM_downBitmap:				ROM_downBitmap,				//@324 [bitmap] big down arrow 
      ROM_leftBitmap:				ROM_leftBitmap,				//@325 [bitmap] these 4 arrows are smaller dimensions
      ROM_rightBitmap:			ROM_rightBitmap,			//@326 [bitmap]
      ROM_upArrowBitmap:			ROM_upArrowBitmap,			//@327 [bitmap]	(2.1 gray?)
      ROM_downArrowBitmap:		ROM_downArrowBitmap,		//@328 [bitmap] (2.1 gray?)
      ROM_overviewBitmap:			ROM_overviewBitmap,			//@329 [bitmap] big dot	(2.1 gray?)
      });
      
      // ===== 330-340
      
      call kAppendFrame with (partData,
      if kBuild20
      then { // 2.0											//@330 (2.1 gray?)
      ROM_keyCapsBitmap:			ROM_keyCapsBitmap,			//@331 [bitmap]
      ROM_keyButtBitmap:			ROM_keyButtBitmap,			//@332 [bitmap]
      //@333 (2.1 gray?)
      ROM_goAwayBitmap:			ROM_goAwayBitmap,			//@334  [bitmap]
      //@335 (2.1 gray?)
      //@336 ?
      //@337 ?
      //@338 ?
      //protoAZVertTabs:			protoAZVertTabs,			//@339 [protNOS2
      ROM_AZTabsMaskA:			ROM_AZTabsMaskA,			//@340 [bitmap]
      }
      
      else { // 1.x
      ROM_globeBitmap:			ROM_globeBitmap,			//@330 [bitmap] compass pts
      ROM_keyCapsBitmap:			ROM_keyCapsBitmap,			//@331 [bitmap] shorter kbd icon
      ROM_keyButtBitmap:			ROM_keyButtBitmap,			//@332 [bitmap] keyboard icon
      ROM_calculatorBitmap:		ROM_calculatorBitmap,		//@333 [bitmap] calculator
      ROM_goAwayBitmap:			ROM_goAwayBitmap,			//@334 [bitmap] small close box
      ROM_paletteBitmap:			ROM_paletteBitmap,			//@335 [bitmap] (styles) palette
      ROM_inboxBitmap:			ROM_inboxBitmap,			//@336 [bitmap] inbox
      ROM_outboxBitmap:			ROM_outboxBitmap,			//@337 [bitmap] outbox
      ROM_dockerBitmap:			ROM_dockerBitmap,			//@338 [bitmap] pc?
      ROM_wizardBitmap:			ROM_wizardBitmap,			//@339 [bitmap] wizard?
      ROM_ntpBitmap:				ROM_ntpBitmap,				//@340 [bitmap] Mac
      });
      
      // ===== 341-350
      Print('rom350);

call kAppendFrame with (partData,
                        if kBuild20
                        then { // 2.0
                               //@341 ?
                        ROM_markupBitmap:			ROM_markupBitmap,			//@342 [bitmap]
                        ROM_noMarkupBitmap:			ROM_noMarkupBitmap,			//@343 [bitmap]
                        ROM_bookmarkBitmap:			ROM_bookmarkBitmap,			//@344 [bitmap]
                                                                                                        //@345 ?
                                                                                                        //@346 onlineBitmap? (2.1 gray?)
                        ROM_cardBitmap:				ROM_cardBitmap,				//@347 [bitmap] (2.1 gray?)
                                                                                                        //@348 ?
                        ROM_a2zBitmap:				ROM_a2zBitmap,				//@349 [bitmap]
                        ROM_zero2nineBitmap:		ROM_zero2nineBitmap,		//@350 [bitmap]
                        }
                        
                        else { // 1.x
                        ROM_clipBitmap:				ROM_clipBitmap,				//@341 [bitmap] paper clip
                        ROM_markupBitmap:			ROM_markupBitmap,			//@342 [bitmap] annotation button
                        ROM_noMarkupBitmap:			ROM_noMarkupBitmap,			//@343 [bitmap] " inverted
                        ROM_bookmarkBitmap:			ROM_bookmarkBitmap,			//@344 [bitmap] turned corner
                        ROM_actionBitmap:			ROM_actionBitmap,			//@345 [bitmap] squiggle? lightning bolt?
                        ROM_onlineBitmap:			ROM_onlineBitmap,			//@346 [bitmap] ringing phone
                        ROM_cardBitmap:				ROM_cardBitmap,				//@347 [bitmap] PCMCIA card icon
                        ROM_dailyBitmap:			ROM_dailyBitmap,			//@348 [bitmap] activities
                        ROM_a2zBitmap:				ROM_a2zBitmap,				//@349 [bitmap] A B C ... Z
                        ROM_zero2nineBitmap:		ROM_zero2nineBitmap,		//@350 [bitmap] 0 1 ... 9
                        });

// ===== 351-360

call kAppendFrame with (partData,
                        if kBuild20
                        then { // 2.0
                        ROM_bookBitmap:				ROM_bookBitmap,				//@351 [bitmap]
                                                                                                        //@352 ?
                                                                                                        //@353 ?
                                                                                                        //@354 ?
                                                                                                        //@355 ?
                                                                                                        //protoDateIntervalPopup:	protoDateIntervalPopup,		//@356 protNOS2
                                                                                                        //protoMultiDatePopup:		protoMultiDatePopup,		//@357 protNOS2
                                                                                                        //protoYearPopup:			protoYearPopup,				//@358 protNOS2
                        ROM_compatibleFinder:		ROM_compatibleFinder,		//@359
                        protoMapPopup:				protoMapPopup,				//@360
                        }
                        
                        else { // 1.x
                        ROM_bookBitmap:				ROM_bookBitmap,				//@351 [bitmap] book icon
                        ROM_switchBitmap:			ROM_switchBitmap,			//@352 [bitmap] <->
                        ROM_assistant:				ROM_assistant,				//@353
                        protoNavigator:				protoNavigator,				//@354
                                                                                                        //@355 ?
                                                                                                        //@356 ?
                                                                                                        //@357 ?
                                                                                                        //@358 ?
                        ROM_compatibleFinder:		ROM_compatibleFinder,		//@359
                        ROM_namesOverview:			ROM_namesOverview,			//@360
                        });


// ===== 361-371
Print('rom370);
      
      call kAppendFrame with (partData,
      if kBuild20
      then { // 2.0											//@361 ?
      //@362 ?
      //@363 ?
      //@364 ?
      //@365 ?
      //@366 ?
      //@367 ?
      //@368 ?
      //@369 ?
      //@370 ?
      //protoPeoplePopup:			protoPeoplePopup,			//@371 protNOS2
      }
      
      else { // 1.x
      ROM_namesEditview:			ROM_namesEditview,			//@361
      ROM_notesOverview:			ROM_notesOverview,			//@362
      ROM_cardAction:				ROM_cardAction,				//@363
      ROM_cardView:				ROM_cardView,				//@364
      //@365 ?
      //@366 ?
      //@367 ?
      //@368 ?
      ROM_namesStylePop:			ROM_namesStylePop,			//@369
      ROM_namesEditGuts:			ROM_namesEditGuts,			//@370
      ROM_tryHardBitmap:			ROM_tryHardBitmap,			//@371
      });
      
      
      // ========= 2.0 ONLY =======
      
      // ===== 372-400
      Print('rom400);

if kBuild20
then call kAppendFrame with (partData,{ // 2.0
                                        //@372 ?
                                        //@373 (2.1 gray?)
                                        //newtRolloverLayout:		newtRolloverLayout,			//@374 protNOS2
                                        //protoAlphaKeyboard:		protoAlphaKeyboard,			//@375 protNOS2
                                        //protoNumericKeyboard:		protoNumericKeyboard,		//@376 protNOS2
                                        //protoPopinPlace:			protoPopinPlace,			//@377 protNOS2
                                        //protoNewSetClock:			protoNewSetClock,			//@378 protNOS2
                                        //protoAMPMCluster:			protoAMPMCluster,			//@379 protNOS2
ROM_copier:					ROM_copier,					//@380 [sound]
ROM_infoBitmap:				ROM_infoBitmap,				//@381 [bitmap]
                                                                                //@382 ?
                                                                                //protoBasicEndpoint:		protoBasicEndpoint,			//@383 protNOS2
                                                                                //@384 ?
                                                                                //protoPrefsRollItem:		protoPrefsRollItem,			//@385 protNOS2
                                                                                //protoPopupButton:			protoPopupButton,			//@386 protNOS2
                                                                                //protoDatePicker:			protoDatePicker,			//@387 protNOS2
                                                                                //@388 ?
                                                                                //protoTransport:			protoTransport,				//@389 protNOS2
                                                                                //@390 ?
                                                                                //@391?
                                                                                //@392?
                                                                                //protoCharEdit:			protoCharEdit,				//@393 protNOS2
                                                                                //@394?
                                                                                //@395?
ROM_AZTabsBitmap:			ROM_AZTabsBitmap,			//@396 [bitmap]
ROM_AZTabsMaskCZ:			ROM_AZTabsMaskCZ,			//@397 [bitmap]
                                                                                //newtApplication:			newtApplication,			//@398 protNOS2
                                                                                //@399?
                                                                                //@400?
});

// ===== 401-430
Print('rom430);
      
      if kBuild20
      then call kAppendFrame with (partData,{ // 2.0
      //newtStatusBar:			newtStatusBar,				//@401 protNOS2
      //newtLayout:				newtLayout,					//@402 protNOS2
      //newtRollLayout:			newtRollLayout,				//@403 protNOS2
      //newtPageLayout:			newtPageLayout,				//@404 protNOS2
      //newtOverLayout:			newtOverLayout,				//@405 protNOS2
      //newtEntryView:			newtEntryView,				//@406 protNOS2
      //newtEntryViewActionButton:newtEntryViewActionButton,	//@407 protNOS2
      //newtEntryViewFilingButton:newtEntryViewFilingButton,	//@408 protNOS2
      //newtRollEntryView:		newtRollEntryView,			//@409 protNOS2
      //newtEntryRollHeader:		newtEntryRollHeader,		//@410 protNOS2
      //newtStationeryView:		newtStationeryView,			//@411 protNOS2
      //newtROEditView:			newtROEditView,				//@412 protNOS2
      //newtEditView:				newtEditView,				//@413 protNOS2
      //newtROTextView:			newtROTextView,				//@414 protNOS2
      //newtTextView:				newtTextView,				//@415 protNOS2
      //newtRONumView:			newtRONumView,				//@416 protNOS2
      //newtNumView:				newtNumView,				//@417 protNOS2
      //newtROTextDateView:		newtROTextDateView,			//@418 protNOS2
      //newtTextDateView:			newtTextDateView,			//@419 protNOS2
      newtQBETextView:			newtQBETextView,			//@420
      //newtROLabelInputLine:		newtROLabelInputLine,		//@421 protNOS2
      //newtLabelInputLine:		newtLabelInputLine,			//@422 protNOS2
      //newtLabelNumInputLine:	newtLabelNumInputLine,		//@423 protNOS2
      //newtLabelDateInputLine:	newtLabelDateInputLine,		//@424 protNOS2
      //newtLabelPhoneInputLine:	newtLabelPhoneInputLine,	//@425 protNOS2
      newtQBELabelInputLine:		newtQBELabelInputLine,		//@426
      //newtSmartNameView:		newtSmartNameView,			//@427 protNOS2
      newtSmartPhoneView:			newtSmartPhoneView,			//@428
      //newtSoup:					newtSoup,					//@429 protNOS2
      //newtAZTabs:				newtAZTabs,					//@430 protNOS2
      });
      
      // ===== 431-460
      Print('rom460);

if kBuild20
then call kAppendFrame with (partData, { // 2.0
                                         //protoSoundChannel:		protoSoundChannel,			//@431 protNOS2
ROM_typewriter:				ROM_typewriter,				//@432 [sound]
                                                                                //@433 ?
                                                                                //protoKeyboardButton:		protoKeyboardButton,		//@434 protNOS2
protoRubricPopup:			protoRubricPopup,			//@435
                                                                                //protoLocationPopup:		protoLocationPopup,			//@436 protNOS2
newtCustomFilter:			newtCustomFilter,			//@437
newtLabelCustomInputLine:	newtLabelCustomInputLine,	//@438
                                                                //newtActionButton:			newtActionButton,			//@439 protNOS2
                                                                //newtFilingButton:			newtFilingButton,			//@440 protNOS2
                                                                //@441 newtROComboView?
ROM_rcNoRecog:				ROM_rcNoRecog,				//@442
ROM_rcPrefsConfig:			ROM_rcPrefsConfig,			//@443
ROM_rcDefaultConfig:		ROM_rcDefaultConfig,		//@444
ROM_rcSingleCharacterConfig:ROM_rcSingleCharacterConfig,//@445
ROM_rcTryLettersConfig:		ROM_rcTryLettersConfig,		//@446
ROM_rcRerecognizeConfig:	ROM_rcRerecognizeConfig,	//@447
ROM_rcBuildChains:			ROM_rcBuildChains,			//@448
                                                                                //ROM_rcInkOrText:			ROM_rcInkOrText,			//@449 protNOS2
protoRecConfig:				protoRecConfig,				//@450
                                                                                //newtStationery:			newtStationery,				//@451 protNOS2
ROM_USStateSoupName:		ROM_USStateSoupName,		//@452 "USStates"
ROM_canadianProvinceName:	ROM_canadianProvinceName,	//@453
                                                                //ROM_continents:			ROM_continents,				//@454
                                                                //protoWorldPicker:			protoWorldPicker,			//@455 protNOS2
                                                                //protoStatePicker:			protoStatePicker,			//@456 protNOS2
                                                                //protoProvincePicker:		protoProvincePicker,		//@457 protNOS2
                                                                //protoCountryPicker:		protoCountryPicker,			//@458 protNOS2
                                                                //newtFloatingBar:			newtFloatingBar,			//@459 protNOS2
                                                                //protoSoupOverView:		protoSoupOverView,			//@460 protNOS2
});

// ===== 461-490
Print('rom490);
      
      if kBuild20
      then call kAppendFrame with (partData, { // 2.0
      //protoListPicker:			protoListPicker,			//@461 protNOS2
      protoDigit:					protoDigit,					//@462 ???
      //protoDigitalClock:		protoDigitalClock,			//@463 protNOS2
      //@464 ?
      //@465 protoListView?										
      //protoStreamingEndpoint:	protoStreamingEndpoint,		//@466 protNOS2
      //protoStatusTemplate:		protoStatusTemplate,		//@467 protNOS2
      //protoStatusClosebox:		protoStatusClosebox,		//@468 protNOS2
      ///protoStatusIcon:			protoStatusIcon,			//@469 protNOS2
      //protoStatusButton:		protoStatusButton,			//@470 protNOS2
      //protoStatusGauge:			protoStatusGauge,			//@471 protNOS2
      //protoStatusProgress:		protoStatusProgress,		//@472 protNOS2
      //protoStatusText:			protoStatusText,			//@473 protNOS2
      //protoOrientation:			protoOrientation,			//@474 protNOS2
      protoHorizontalUpDownScroller:
      protoHorizontalUpDownScroller, 	//@475 protNOS2
      //@476 ?
      //protoTransportHeader:		protoTransportHeader,		//@477 protNOS2
      //protoInfoButton:			protoInfoButton,			//@478 protNOS2
      //@479 (2.1 gray?)
      //@480 ?
      //protoThumbnailFloater:	protoThumbnailFloater,		//@481 protNOS2
      //@482 ?
      //@483 ?
      //protoThumbnail:			protoThumbnail,				//@484 protNOS2
      //protoImageView:			protoImageView,				//@485 protNOS2
      //@486 ?
      //protoFormatPicker:		protoFormatPicker,			//@487 protNOS2
      //protoSendButton:			protoSendButton,			//@488 protNOS2
      //@489 ?
      //@490 ?
      });
      
      // ===== 491-520
      Print('rom520);

if kBuild20
then call kAppendFrame with (partData, { // 2.0
                                         //newtROTextTimeView:		newtROTextTimeView,			//@491 protNOS2
                                         //newtTextTimeView:			newtTextTimeView,			//@492 protNOS2
                                         //newtLabelTimeInputLine:	newtLabelTimeInputLine,		//@493 protNOS2
newtROSymbolView:			newtROSymbolView,			//@494
newtSymbolView:				newtSymbolView,				//@495
newtLabelSymbolInputLine:	newtLabelSymbolInputLine,	//@496
                                                                //protoPersonaPopup:			protoPersonaPopup,		//@497 protNOS2
                                                                //protoEmporiumPopup:		protoEmporiumPopup,			//@498 protNOS2
                                                                //@499 ?
                                                                //@500 ?
                                                                //@501 ?
                                                                //@502 ?
                                                                //@503 ?
protoDisplayClock:			protoDisplayClock,			//@504
                                                                                //@505 ?
                                                                                //@506 ?
                                                                                //newtEntryLockedIcon:		newtEntryLockedIcon,		//@507 protNOS2
                                                                                //@508 ?
                                                                                //@509 ?
                                                                                //@510 newtPopupEdit?
                                                                                //@511 ?
protoLocationPicker:		protoLocationPicker,		//@512
ROM_AustralianStateName:	ROM_AustralianStateName,	//@513
protoDigitalTimePopup:		protoDigitalTimePopup,		//@514
ROM_faxRoutingIcon:			ROM_faxRoutingIcon,			//@515 [bitmap]
protoTimeDeltaPicker:		protoTimeDeltaPicker,		//@516
                                                                //protoTimeDeltaPopup:		protoTimeDeltaPopup,		//@517 protNOS2
protoLatitudePicker:		protoLatitudePicker,		//@518
protoLongitudePicker:		protoLongitudePicker,		//@519
protoDigitBase:				protoDigitBase,				//@520 ?
});

// ===== 521-550
Print('rom550);
      
      if kBuild20
      then call kAppendFrame with (partData, { // 2.0
      protoLongLatPicker:			protoLongLatPicker,			//@521
      //protoTimeDeltaTextPicker:	protoTimeDeltaTextPicker,	//@522 protNOS2
      //protoLongLatTextPicker:	protoLongLatTextPicker,		//@523 protNOS2
      //ROM_UKCountyName:			ROM_UKCountyName,			//@524
      //@525 ?
      //@526 ?
      //@527 ?
      //@528 ?
      //@529 ?
      //@530 ?
      //@531 ?
      //@532 ?
      //@533 ?
      //@534 ?
      //@535 ?
      protoAliasCursor:			protoAliasCursor,			//@536
      //@537 ?
      //@538 ?
      //@539 ?
      //@540 ?
      //@541 ?
      //@542 ?
      //@543 ?
      //@544 ?
      //@545 ?
      //@546 ?
      //@547 ?
      //@548 ?
      //@549 ?
      //@550 ?
      });
      
      // ===== 551-580
      Print('rom580);

if kBuild20
then call kAppendFrame with (partData, { // 2.0
                                         //@551 ?
                                         //protoBlindEntryLine:		protoBlindEntryLine,		//@552 protNOS2 //ROM_productLogoBitmap:	ROM_productLogoBitmap,		//@552 [bitmap]??
protoCountryLocPicker:		protoCountryLocPicker,		//@553
protoStateLocPicker:		protoStateLocPicker,		//@554
protoDateNYearPicker:		protoDateNYearPicker,		//@555
                                                                //@556 ?
                                                                //@557 ?
                                                                //@558 ?
protoMultiLinePicker:		protoMultiLinePicker,		//@559
ROM_azTabsSlimBitmap:		ROM_azTabsSlimBitmap,		//@560 [bitmap]
ROM_aztabsSlimMaskCZ:		ROM_aztabsSlimMaskCZ,		//@561 [bitmap]???
ROM_aztabsSlimMaskA:		ROM_aztabsSlimMaskA,		//@562 [bitmap]???
                                                                //protoTitleText:			protoTitleText,				//@563 protNOS2
newtProtoLineBase:			newtProtoLineBase,			//@564
                                                                                //newtProtoLine:			newtProtoLine,				//@565 protNOS2
                                                                                //@566 ?
                                                                                //@567 ?
                                                                                //@568 ?
                                                                                //@569 ?
                                                                                //@570 ?
                                                                                //@571 ?
                                                                                //@572 ?
                                                                                //@573 ?
                                                                                //@574 ?
                                                                                //@575 ?
                                                                                //@576 ?
                                                                                //@577 ?
                                                                                //@578 ?
                                                                                //@579 ?
                                                                                //@580 ?
});

// ===== 581-610
Print('rom610);
      
      if kBuild20
      then call kAppendFrame with (partData, { // 2.0
      //@581 ?
      //@582 ?
      //@583 ?
      //@584 ?
      //@585 ?
      //@586 ?
      newtPhonePopupEdit:			newtPhonePopupEdit,			//@587
      //ROM_DefRotateFunc:		ROM_DefRotateFunc,			//@588 protNOS2
      //@589 ?
      //@590 ?
      ROM_alarmIconTinyBitmap:	ROM_alarmIconTinyBitmap,	//@591 [bitmap]
      //@592 (2.1 gray?)
      ROM_helpbookBitmap:			ROM_helpbookBitmap,			//@593 [bitmap]
      ROM_diamanteBitmap:			ROM_diamanteBitmap,			//@594 [bitmap]
      //@595 ?
      //@596 ?
      //@597 ?
      //@598 ?
      //@599 ?
      ROM_openPadlockBitmap:		ROM_openPadlockBitmap,		//@600 [bitmap]
      ROM_closedPadlockBitmap:	ROM_closedPadlockBitmap,	//@601 [bitmap]
      //@602 (2.1 gray?)
      ROM_colonSlides:			ROM_colonSlides,			//@603 [bitmap]
      ROM_ampmSlides:				ROM_ampmSlides,				//@604 [bitmap]
      //@605 protoDayPicker?
      ROM_canonicalCharGrid:		ROM_canonicalCharGrid,		//@606
      //protoDateDurationTextPicker: protoDateDurationTextPicker,//@607 protNOS2
      //protoHorizontal2DScroller:protoHorizontal2DScroller,	//@608 protNOS2
      protoLogPicker:				protoLogPicker,				//@609
      //@610 ?
      });
      
      // ===== 611-640
      Print('rom640);

if kBuild20
then call kAppendFrame with (partData, { // 2.0
                                         //@611 ?
                                         //@612 ?
                                         //@613 ?
protoCorrectInfo:			protoCorrectInfo,			//@614
protoWordInfo:				protoWordInfo,				//@615
protoWordInterp:			protoWordInterp,			//@616
                                                                                //protoAZTabs:				protoAZTabs,				//@617 protNOS2
                                                                                //newtCheckBox:				newtCheckBox,				//@618 protNOS2
                                                                                //newtROLabelNumInputLine:	newtROLabelNumInputLine,	//@619 protNOS2
                                                                                //newtROLabelDateInputLine:	newtROLabelDateInputLine,	//@620 protNOS2
                                                                                //newtROLabelTimeInputLine:	newtROLabelTimeInputLine,	//@621 protNOS2
ROM_digitSlides:			ROM_digitSlides,			//@622 [bitmap]
protoDesktopEndpoint:		protoDesktopEndpoint,		//@623
                                                                //protoSmallKeyboardButton:	protoSmallKeyboardButton,	//@624 protNOS2
protoDiamondButton:			protoDiamondButton,			//@625
                                                                                //protoTextPicker:			protoTextPicker,			//@626 protNOS2
                                                                                //protoTimeTextPicker:		protoTimeTextPicker,		//@627 protNOS2
                                                                                //protoDurationTextPicker:	protoDurationTextPicker,	//@628 protNOS2
                                                                                //protoDateTextPicker:		protoDateTextPicker,		//@629 protNOS2
                                                                                //protoDateNTimeTextPicker:	protoDateNTimeTextPicker,	//@630 protNOS2
                                                                                //protoMapTextPicker:		protoMapTextPicker,			//@631 protNOS2
                                                                                //protoCountryTextPicker:	protoCountryTextPicker,		//@632 protNOS2
                                                                                //protoUSStatesTextPicker:	protoUSStatesTextPicker,	//@633 protNOS2
                                                                                //protoCitiesTextPicker:	protoCitiesTextPicker,		//@634 protNOS2
                                                                                //newtNRLabelTimeInputLine:	newtNRLabelTimeInputLine,	//@635 protNOS2
                                                                                //newtNRLabelDateInputLine:	newtNRLabelDateInputLine,	//@636 protNOS2
                                                                                //newtNRLabelDateNTimeInputLine:
                                                                                //newtNRLabelDateNTimeInputLine,//@637 protNOS2
                                                                                //newtFilter:				newtFilter,					//@638 protNOS2
                                                                                //newtTextFilter:			newtTextFilter,				//@639 protNOS2
                                                                                //newtIntegerFilter:		newtIntegerFilter,			//@640 protNOS2
});

// ===== 641-670
Print('rom670);
      
      if kBuild20
      then call kAppendFrame with (partData, { // 2.0
      //newtNumberFilter:			newtNumberFilter,			//@641 protNOS2
      //newtDateFilter:			newtDateFilter,				//@642 protNOS2
      //newtTimeFilter:			newtTimeFilter,				//@643 protNOS2
      //newtDateNTimeFilter:		newtDateNTimeFilter,		//@644 protNOS2
      newtSymbolFilter:			newtSymbolFilter,			//@645
      //newtPhoneFilter:			newtPhoneFilter,			//@646 protNOS2
      //newtCityFilter:			newtCityFilter,				//@647 protNOS2
      //newtStateFilter:			newtStateFilter,			//@648 protNOS2
      //newtCountryFilter:		newtCountryFilter,			//@649 protNOS2
      //newtSmartNameFilter:		newtSmartNameFilter,		//@650 protNOS2
      //ROM_flapSlides:			ROM_flapSlides,				//@651 [bitmap]
      //newtFalseEntryView:		newtFalseEntryView,			//@652 protNOS2
      //@653 ?
      //@654 ?
      //protoFullRouteSlip:		protoFullRouteSlip,			//@655 protNOS2
      //protoUpDownScroller:		protoUpDownScroller,		//@656 protNOS2
      //protoLeftRightScroller:	protoLeftRightScroller,		//@657 protNOS2
      ROM_dstSoupName:			ROM_dstSoupName,			//@658
      newtSmartCompanyFilter:		newtSmartCompanyFilter,		//@659
      newtSmartAddressFilter:		newtSmartAddressFilter,		//@660
      //@661 ?
      //@662 ?
      //@663 ?
      //protoPeoplePicker:		protoPeoplePicker,			//@664 protNOS2
      //@665 ?
      //@666 newtEmbeddedEntryView?
      //@667 ?
      //@668 ?
      //protoNewFolderTab:		protoNewFolderTab,			//@669 protNOS2
      //protoClockFolderTab:		protoClockFolderTab,		//@670 protNOS2
      });
      
      // ===== 671-700
      Print('rom700);

if kBuild20
then call kAppendFrame with (partData, { // 2.0
                                         //protoGeneralPopup:		protoGeneralPopup,			//@671 protNOS2
                                         //newtInfoBox:				newtInfoBox,				//@672 protNOS2
                                         //@673 ?
                                         //protoRichInputLine:		protoRichInputLine,			//@674 protNOS2
                                         //protoRichLabelInputLine:	protoRichLabelInputLine,	//@675 protNOS2
                                         //protoPhonePad:			protoPhonePad,				//@676 protNOS2
                                         //protoTouchTonePad:		protoTouchTonePad,			//@677 protNOS2
                                         //protoTransportPrefs:		protoTransportPrefs,		//@678 protNOS2
                                         //@679?
                                         //@680?
ROM_canonicalBaseInfo:		ROM_canonicalBaseInfo,		//@681
                                                                //newtLabelSimpleDateInputLine: newtLabelSimpleDateInputLine,//@682 protNOS2
                                                                //newtSimpleDateFilter:		newtSimpleDateFilter,		//@683 protNOS2
                                                                //@684 ?
ROM_digitFlapLowOrd:		ROM_digitFlapLowOrd,		//@685 [bitmap]
ROM_digitFlap:				ROM_digitFlap,				//@686 [bitmap]
ROM_digitWideFlap:			ROM_digitWideFlap,			//@687 [bitmap]
ROM_plusMinusBitmap:		ROM_plusMinusBitmap,		//@688 [bitmap]
ROM_northSouthBitmap:		ROM_northSouthBitmap,		//@689 [bitmap]
ROM_eastWestBitmap:			ROM_eastWestBitmap,			//@690 [bitmap]
ROM_draggeurBitmap:			ROM_draggeurBitmap,			//@691 [bitmap]
ROM_draggeurNoGrayBitmap:	ROM_draggeurNoGrayBitmap,	//@692 [bitmap]
ROM_dtmf0Bitmap:			ROM_dtmf0Bitmap,			//@693 [bitmap]
ROM_dtmf1Bitmap:			ROM_dtmf1Bitmap,			//@694 [bitmap]
ROM_dtmf2Bitmap:			ROM_dtmf2Bitmap,			//@695 [bitmap]
ROM_dtmf3Bitmap:			ROM_dtmf3Bitmap,			//@696 [bitmap]
ROM_dtmf4Bitmap:			ROM_dtmf4Bitmap,			//@697 [bitmap]
ROM_dtmf5Bitmap:			ROM_dtmf5Bitmap,			//@698 [bitmap]
ROM_dtmf6Bitmap:			ROM_dtmf6Bitmap,			//@699 [bitmap]
ROM_dtmf7Bitmap:			ROM_dtmf7Bitmap,			//@700 [bitmap]
});

// ===== 701-730
Print('rom730);
      
      if kBuild20
      then call kAppendFrame with (partData, { // 2.0
      ROM_dtmf8Bitmap:			ROM_dtmf8Bitmap,			//@701 [bitmap]
      ROM_dtmf9Bitmap:			ROM_dtmf9Bitmap,			//@702 [bitmap]
      ROM_dtmfStarBitmap:			ROM_dtmfStarBitmap,			//@703 [bitmap]
      ROM_dtmfPoundBitmap:		ROM_dtmfPoundBitmap,		//@704 [bitmap]
      //@705 (2.1 gray?)
      //@706 (2.1 gray?)
      ROM_finePenTipBitmap:		ROM_finePenTipBitmap,		//@707 [bitmap]
      ROM_smallPenTipBitmap:		ROM_smallPenTipBitmap,		//@708 [bitmap]
      ROM_mediumPenTipBitmap:		ROM_mediumPenTipBitmap,		//@709 [bitmap]
      ROM_largePenTipBitmap:		ROM_largePenTipBitmap,		//@710 [bitmap]
      //@711 ?
      //@712 ?
      ROM_handScrollBitmap:		ROM_handScrollBitmap,		//@713 [bitmap]
      ROM_keyButtonBitmap:		ROM_keyButtonBitmap,		//@714 [bitmap]
      ROM_keySmallButtonBitmap:	ROM_keySmallButtonBitmap,	//@715 [bitmap]
      ROM_keyPadBitmap:			ROM_keyPadBitmap,			//@716 [bitmap]
      //@717 ?
      //@718 ?
      //@719 (2.1 gray?)
      //@720 ?
      //@721 ?
      //@722 ?
      //@723 ?
      //@724 ?
      //@725 ?
      //@726 ?
      //@727 ?
      //@728 ?
      //@729 ?
      ROM_routePrintIcon:			ROM_routePrintIcon,			//@730 [bitmap]
      });
      
      // ===== 731-760
      Print('rom760);

if kBuild20
then call kAppendFrame with (partData, { // 2.0
ROM_routeBeamIcon:			ROM_routeBeamIcon,			//@731 [bitmap]
ROM_routeMailIcon:			ROM_routeMailIcon,			//@732 [bitmap]
                                                                                //@733 (2.1 gray?)
ROM_keyBulletBitmap:		ROM_keyBulletBitmap,		//@734 [bitmap]
ROM_keyColonBitmap:			ROM_keyColonBitmap,			//@735 [bitmap]
ROM_keyCommaBitmap:			ROM_keyCommaBitmap,			//@736 [bitmap]
ROM_keyDivideBitmap:		ROM_keyDivideBitmap,		//@737 [bitmap]
ROM_keyEqualsBitmap:		ROM_keyEqualsBitmap,		//@738 [bitmap]
ROM_keyLeftParenBitmap:		ROM_keyLeftParenBitmap,		//@739 [bitmap]
ROM_keyMinusBitmap:			ROM_keyMinusBitmap,			//@740 [bitmap]
ROM_keyPeriodBitmap:		ROM_keyPeriodBitmap,		//@741 [bitmap]
ROM_keyPlusBitmap:			ROM_keyPlusBitmap,			//@742 [bitmap]
ROM_keyRadicalBitmap:		ROM_keyRadicalBitmap,		//@743 [bitmap]
ROM_keyRightParenBitmap:	ROM_keyRightParenBitmap,	//@744 [bitmap]
ROM_keySlashBitmap:			ROM_keySlashBitmap,			//@745 [bitmap]
ROM_keyTimesBitmap:			ROM_keyTimesBitmap,			//@746 [bitmap]
ROM_kbdReturnBitmap:		ROM_kbdReturnBitmap,		//@747 [bitmap]
ROM_kbdTabBitmap:			ROM_kbdTabBitmap,			//@748 [bitmap]
ROM_kbdLeftBitmap:			ROM_kbdLeftBitmap,			//@749 [bitmap]
ROM_kbdRightBitmap:			ROM_kbdRightBitmap,			//@750 [bitmap]
ROM_kbdDictBitmap:			ROM_kbdDictBitmap,			//@751 [bitmap]
ROM_kbdDeleteBitmap:		ROM_kbdDeleteBitmap,		//@752 [bitmap]
ROM_kbdShiftBitmap:			ROM_kbdShiftBitmap,			//@753 [bitmap]
ROM_kbdOptionBitmap:		ROM_kbdOptionBitmap,		//@754 [bitmap]
ROM_kbdcapslockBitmap:		ROM_kbdcapslockBitmap,		//@755 [bitmap]
ROM_routeReply:				ROM_routeReply,				//@756 [bitmap]
ROM_routeReaddress:			ROM_routeReaddress,			//@757 [bitmap]
ROM_routePutAway:			ROM_routePutAway,			//@758 [bitmap]
ROM_routeLog:				ROM_routeLog,				//@759 [bitmap]
ROM_routeCallBitmap:		ROM_routeCallBitmap,		//@760 [bitmap]
});

// ===== 761-790
Print('rom790);
      
      if kBuild20
      then call kAppendFrame with (partData, { // 2.0
      ROM_routeUpdateBitmap:		ROM_routeUpdateBitmap,		//@761 [bitmap]
      //@762 ?
      ROM_checkBitmap:			ROM_checkBitmap,			//@763 [bitmap]
      //@764 (2.1 gray?)
      ROM_notesBitmap:			ROM_notesBitmap,			//@765 [bitmap] (2.1 gray?)
      ROM_topicMarkers:			ROM_topicMarkers,			//@766
      //@767 ?
      ROM_routeFaxIcon:			ROM_routeFaxIcon,			//@768 [bitmap]
      ROM_routeTransport:			ROM_routeTransport,			//@769 [bitmap]
      ROM_routeMissing:			ROM_routeMissing,			//@770 [bitmap]
      ROM_routeForward:			ROM_routeForward,			//@771 [bitmap]
      ROM_routeAddSender:			ROM_routeAddSender,			//@772 [bitmap]
      ROM_routePasteText:			ROM_routePasteText,			//@773 [bitmap]
      ROM_transportScripts:		ROM_transportScripts,		//@774 [bitmap]
      //@775 ?
      //@776 ?
      //@777 ?
      //@778 ?
      //@779 ?
      //@780 ?
      //@781 ?
      //@782 ?
      //@783 ?
      //@784 ?
      //@785 ?
      //@786 ?
      //@787 ?
      //@788 ?
      //@789 ?
      //@790 ?
      });
      
      // ===== 791-816
      Print('rom810);

if kBuild20
then call kAppendFrame with (partData, { // 2.0
                                         //@791 ?
                                         //@792 ?
                                         //@793 ?
                                         //@794 ?
                                         //@795 ?
                                         //@796 ?
                                         //@797 ?
                                         //@798 ?
                                         //@799 ?
                                         //@800 ?
                                         //@801 ?
                                         //@802 ?
                                         //@803 ?
                                         //protoDragNGo:				protoDragNGo,				//@804 protNOS2
                                         //@805 ?
                                         //@806 ?
                                         //@807 ?
                                         //@808 ?
                                         //@809 ?
                                         //@810 ?
                                         //@811 ?
                                         //newtStationeryPopupButton:	newtStationeryPopupButton,		//@812 protNOS2
                                         //newtNewStationeryButton:		newtNewStationeryButton,		//@813 protNOS2
                                         //newtShowStationeryButton:		newtShowStationeryButton,		//@814 protNOS2
                                         //newtRollShowStationeryButton: newtRollShowStationeryButton,	//@815 protNOS2
                                         //newtEntryShowStationeryButton:newtEntryShowStationeryButton,	//@816 protNOS2
});


if kBuild20
then call kAppendFrame with (partData, {
  // 2.1
  
  //protoTXView:				protoTXView,				//@826 protNOS2
  //protoTXViewFinder:		protoTXViewFinder,			//@827 protNOS2
  //ROM_GetSerialNumber:		ROM_GetSerialNumber,		//@846
  //protoSoundFrame:			protoSoundFrame,			//@849 protNOS2
  //protoRecorderView:		protoRecorderView,			//@853 protNOS2
  // newtCheckAllButton = @872 ?
  
*/

#endif