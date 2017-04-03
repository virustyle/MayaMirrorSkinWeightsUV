/* Copyright (C) 2017 Marc Wilhite - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the MIT license.
 */

#include "MirrorSkinWeightsUV.h"

// Maya ------------------------------------------------------------------------

#include <maya/MFnPlugin.h>

// =============================================================================

// =============================================================================

const char *const VENDOR = "Marc Wilhite";
const char *const VERSION = "1.0";

// -----------------------------------------------------------------------------

MStatus initializePlugin( MObject obj )
    //
    //  Description:
    //      this method is called when the plug-in is loaded into Maya.
    //      It registers all of the services that this plug-in provides with Maya.
    //
    //  Arguments:
    //      obj - a handle to the plug-in object (use MFnPlugin to access it)
    //
{
    MStatus status = MStatus::kFailure;
    MFnPlugin plugin( obj, VENDOR, VERSION, "Any" );


    status = plugin.registerCommand("mirrorSkinWeightsUV",
        MirrorSkinWeightsUV::creator);

    if( status == MStatus::kFailure )
    {
        status.perror( "registerCommand mirrorSkinWeightsUV" );
        return( status );
    }

    return status;
}

MStatus uninitializePlugin( MObject obj )
    //
    //  Description:
    //      this method is called when the plug-in is unloaded from Maya.
    //      It de-registers all of the services that it was providing.
    //
    //  Arguments:
    //      obj - a handle to the plug-in object (use MFnPlugin to access it)
    //
{
    MStatus status = MStatus::kFailure;
    MFnPlugin plugin( obj );

    status = plugin.deregisterCommand("mirrorSkinWeightsUV");

    if( status == MStatus::kFailure )
    {
        status.perror( "deregisterCommand mirrorSkinWeightsUV" );
        return( status );
    }

    return status;
}
