// Maya ------------------------------------------------------------------------
#include <maya/MPxCommand.h>
#include <maya/MIOStream.h>
#include <maya/MString.h>
#include <maya/MGlobal.h>
#include <maya/MArgList.h>
#include <maya/MArgDatabase.h>
#include <maya/MSyntax.h>
#include <maya/MSelectionList.h>
#include <maya/MItMeshVertex.h>
#include <maya/MFnMesh.h>
#include <maya/MFnSingleIndexedComponent.h>
#include <maya/MDoubleArray.h>
#include <maya/MIntArray.h>
#include <maya/MCommandResult.h>
#include <maya/MDagPathArray.h>
#include <maya/MDagPath.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MComputation.h>
#include <maya/MFnSkinCluster.h>

// Standard Library ------------------------------------------------------------

#include <regex>
#include <cstdint>

// =============================================================================

class MirrorSkinWeightsUV : public MPxCommand
{
    static const MString kMirrorInverseFlag;
    static const MString kMirrorInversetFlagLong;
    static const MString kLeftPrefixFlag;
    static const MString kLeftPrefixFlagLong;
    static const MString kRightPrefixFlag;
    static const MString kRightPrefixFlagLong;
    static const MString kToleranceFlag;
    static const MString kToleranceFlagLong;
    static const MString kThresholdFlag;
    static const MString kThresholdFlagLong;
    static const MString kExcludeMatchFlag;
    static const MString kExcludeMatchFlagLong;
    static const MString kAxisOnUFlag;
    static const MString kAxisOnUFlagLong;
    static const MString kuvSetNameFlag;
    static const MString kuvSetNameFlagLong;
    static const MString kHelpFlag;
    static const MString kHelpFlagLong;
    static const MString kLeftPrefix;
    static const MString kRightPrefix;

public:
    MirrorSkinWeightsUV();
    ~MirrorSkinWeightsUV();
    MStatus           doIt(const MArgList& args) override;
    MStatus           undoIt() override;
    MStatus           redoIt() override;
    bool              isUndoable() const override { return true; }

    static  void* creator();

private:
    bool               sortUV(double x, double y, bool z) const;
    MObject            findSkinCluster(const MDagPath& skinClusterDagPath) const;
    int32_t            findIndex(int indexKey, const MIntArray &influenceIds) const;
    float              getDistance(float2 uvCoordA, float2 uvCoordB) const;

    MObject            skinCluster;
    MDagPath		   meshDagPath;
    MObject			   meshComponent;
    MStringArray	   excludeMatches;
    MString			   leftPrefix;
    MString			   rightPrefix;
    MString			   uvCurrentSet;

    MDoubleArray       oldWeights;
    MIntArray          indices;
    MObject            targetComponents;

    double             tolerance;
    double             threshold;
    double             axis;
    bool               leftToRight;


};


