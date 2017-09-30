Tobii EyeX Software Development Kit for C/C++
=============================================

README

  This package contains everything a developer needs for building games
  and applications using the Tobii EyeX Engine API and the C and C++
  programming languages: the C header files for accessing the API, libraries,
  documentation, and code samples.

  Note that Tobii offers several SDK packages targeted at different programming
  languages and frameworks, so be sure to pick the one that fits your needs best.

CONTACT

  If you have problems, questions, ideas, or suggestions, please use the forums
  on the Tobii Developer Zone (link below). That's what they are for!

WEB SITE

  Visit the Tobii Developer Zone web site for the latest news and downloads:

  http://developer.tobii.com/

COMPATIBILITY

  This version of the EyeX SDK requires EyeX Engine version 1.0 or later.
  Specific features will require newer versions of the EyeX Engine as listed
  in the revision history below.

REVISION HISTORY

  2016-03-14
  Version 1.7: No changes.

  2015-11-19
  Version 1.6:
    - Removed all dependencies on the EyeXButton for activatable behavior. The
      samples using the activatable behavior now hook their own keyboard keys
      for activation, and send action commands to trigger activation. The 
      board game sample also illustrates how to trigger activation mode on.
    - Rewrote and restructured parts of the Developer Guide, and added more 
      detailed inforation about action commands for the activatable and the
      pannable behaviors.

  2015-06-12
  Version 1.5:
    - Added support for new EyeTracking state TX_STATEPATH_GAZETRACKING. Updated
      the MinimalStatusNotifications sample to include this state. Requires EyeX 
      Engine 1.4.0.
    - Added support for new states introduced with EyeX Engine 1.3.0. Some of 
      the new states replace deprecated states. The new state path names
      correspond to a logical hierarchy where a number of states are sorted 
      under so called root nodes (for example TX_STATEPATH_EYETRACKING). The 
      list below is incomplete, for a full list of available state paths and 
      their descriptions, see EyeXLiterals.h. Samples have been updated to use 
      the new states where applicable.
        
      New state                                   | Replaces deprecated state
      ------------------------------------------- | --------------------------------
      TX_STATEPATH_EYETRACKINGSCREENBOUNDS        | TX_STATEPATH_SCREENBOUNDS *)
      TX_STATEPATH_EYETRACKINGDISPLAYSIZE         | TX_STATEPATH_DISPLAYSIZE *)
      TX_STATEPATH_EYETRACKINGCONFIGURATIONSTATUS | TX_STATEPATH_CONFIGURATIONSTATUS *)
      TX_STATEPATH_ENGINEINFOVERSION              | TX_STATEPATH_ENGINEVERSION **)
      TX_STATEPATH_EYETRACKINGCURRENTPROFILENAME  | TX_STATEPATH_PROFILENAME **)
      TX_STATEPATH_EYETRACKINGPROFILES            |
      TX_STATEPATH_EYETRACKINGINFO                |
      TX_STATEPATH_EYETRACKINGINFOMODELNAME       |
      TX_STATEPATH_EYETRACKINGINFOSERIALNUMBER    |
      TX_STATEPATH_EYETRACKINGINFOFIRMWAREVERSION |

       *) The new state path is identical to the old, only the name differs. This 
          new state path is backwards compatible with older EyeX Engines.
      **) The new state path is different from the old. You may want to keep 
          using the old path for a while for compatibility with pre-1.3.0 EyeX
          Engines. 
      
    - Added support for new EyeX Engine API function: txSetCurrentProfile.
      Requires EyeX Engine 1.3.0.   
    - Added a MinimalUserProfiles sample to demonstrate how to list profiles
      and change current profile. Requires EyeX Engine 1.3.0.
    - New semantic behavior for TX_STATEPATH_USERPRESENCE state: the user will 
      be detected as present in more cases than before. The user's eyes do not 
      have to be open.
    - New enum value for the TX_STATEPATH_USERPRESENCE state: 
      TX_USERPRESENCE_UNKNOWN.

  2015-04-14
  Version 1.4: No changes. 

  2015-01-15
  Version 1.3: No changes.

  2014-12-16
  Version 1.2:
    - Added support for launching EyeX configuration tools. This function 
      requires EyeX Engine 1.1.
    - New code sample to demonstrate the EyePosition data stream.
    - New EyeX Engine API function: txGetEyeXAvailability. This function is 
      implemented in the client library and works with all Engine versions.
    - Added a check for engine availability to the MinimalStatusNotifications 
      sample app.

  2014-11-20
  Version 1.1:
    - Improved error handling in the shutdown sequence of the C samples.

  2014-10-27
  Version 1.0:
    - Client library compatible with EyeX Engine 1.0.
    - Updated the MinimalStatusNotifications sample to use asynchronous state 
      queries in the engine-connection-state-changed event handler.
    - Updated samples and documentation for the new direct click modes in 
      EyeX Interaction settings.

  2014-09-23
  Version 0.32: Client library compatible with both EyeX Engine 0.10 and 1.0.

  2014-09-05
  Version 0.31: Updated package for Tobii EyeX Engine 0.10.0:
    - Client libraries updated with some breaking API changes (see below).
    - All samples are updated to the new client libraries.
    
  2014-08-22
  Version 0.24: No changes.

  2014-06-19
  Version 0.23: Improved the readability of the API header files by expanding 
  some of the macros.

  2014-05-21
  Version 0.22: Improvements to the Developer's Guide. Bug fixes in the client 
  library.

  2014-05-07
  Version 0.21: Updated package for Tobii EyeX Engine 0.8.14:
    - Client libraries updated with some breaking API changes (see below).
    - All samples are updated to the new client libraries.
    - Improvements to the C/C++ code samples.

  2014-04-08
  Version 0.20: Updated package for Tobii EyeX Engine 0.8.11:
    - Client libraries updated with some breaking API changes (see below).
    - All samples are updated to the new client libraries.
    - MinimalFixationDataStream sample now works as expected.
    - MinimalStatusNotifications sample now also displays presence data.
    - The Developer's Guide is updated.

  2014-03-05
  Version 0.17: Changes to the custom threading and logging API. Added the
  txEnableMonoCallbacks function.

  2014-02-28
  Version 0.16: Added additional notification handlers in the
  MinimalStatusNotifications sample to show how to retrieve display size and
  screen bounds settings. Added new experimental sample to demonstrate the
  Fixation data stream.

  2014-02-26
  Version 0.15: No changes.

  2014-02-21
  Version 0.14.40: Minor improvements.

  2014-02-12
  Version 0.13.39: Bug fixes: Settings retrieval bug fixed in client library.

  2014-02-06
  Version 0.13.38: Added samples licence agreement. Added missing copyright
  texts to C++ binding.

  2014-01-03
  Version 0.13.37: This is the first official alpha release of the SDK. APIs
  may change and backward compatibility isn't guaranteed. As a rule of thumb,
  the APIs used in the samples are the most mature and less likely to change
  much.

EYEX ENGINE API CHANGES

  2014-10-22
  EyeX Engine 1.0
  - No actual API changes, but functional changes related to the Activatable 
    behavior, direct click and key bindings: 
       - If EyeX Interaction is disabled, no default keys are mapped to direct click.
       - ActivationFocus and Activated events are sent simultaneously if EyeX Button 
         interaction is configured in EyeX Interaction settings.

  2014-09-05
  EyeX Engine Developer Preview 0.10.0
  - Name changes:
      TX_STATEPATH_STATE => TX_STATEPATH_EYETRACKINGSTATE
      TX_STATEPATH_PRESENCEDATA => TX_STATEPATH_USERPRESENCE
      TX_PRESENCEDATA_PRESENT => TX_USERPRESENCE_PRESENT
      txInitializeSystem => txInitializeEyeX
      txSet[Xyz]Behavior => txCreate[Xyz]Behavior
      TX_SYSTEMCOMPONENTOVERRIDEFLAG* => TX_EYEXCOMPONENTOVERRIDEFLAG*
      TX_INTERACTIONBEHAVIORTYPE* => TX_BEHAVIORTYPE*
  - Features that are tentative are now documented as "internal" and may 
    disappear in future releases.
  - txInitializeEyeX takes an additional parameter. Currently it is only a 
    placeholder, so pass in a null pointer.
  - The handle passed to txCreateContext must be initialized to TX_EMPTY_HANDLE.
  - Registration methods may no longer be called from API callbacks.
  - The ScopedConstHandle type has been removed.
  - More than one state observer can now be registered for the same state path.
  
  2014-05-07
  EyeX Engine Developer Preview 0.8.14
  - AsyncData objects are used also in query and event handlers.
  - Type of AsyncData objects is of type TX_CONSTHANDLE and should not be released.
  - The third and last parameter of txUnregisterStateChangedHandler has been removed.
  - The signature of the txInitializeSystem function has been changed.
