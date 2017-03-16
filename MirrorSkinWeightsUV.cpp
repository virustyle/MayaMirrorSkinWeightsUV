#include "MirrorSkinWeightsUV.h"

#include <limits>
#include <cfloat>

const MString MirrorSkinWeightsUV::kMirrorInverseFlag       = "-mi";
const MString MirrorSkinWeightsUV::kMirrorInversetFlagLong  = "-mirrorInverse";
const MString MirrorSkinWeightsUV::kLeftPrefixFlag          = "-lp";
const MString MirrorSkinWeightsUV::kLeftPrefixFlagLong      = "-leftPrefix";
const MString MirrorSkinWeightsUV::kRightPrefixFlag         = "-rp";
const MString MirrorSkinWeightsUV::kRightPrefixFlagLong     = "-rightPrefix";
const MString MirrorSkinWeightsUV::kToleranceFlag           = "-tol";
const MString MirrorSkinWeightsUV::kToleranceFlagLong       = "-tolerance";
const MString MirrorSkinWeightsUV::kThresholdFlag           = "-thr";
const MString MirrorSkinWeightsUV::kThresholdFlagLong       = "-threshold";
const MString MirrorSkinWeightsUV::kExcludeMatchFlag        = "-exc";
const MString MirrorSkinWeightsUV::kExcludeMatchFlagLong    = "-excludeMatch";
const MString MirrorSkinWeightsUV::kAxisOnUFlag             = "-u";
const MString MirrorSkinWeightsUV::kAxisOnUFlagLong         = "-axisOnU";
const MString MirrorSkinWeightsUV::kuvSetNameFlag           = "-uvs";
const MString MirrorSkinWeightsUV::kuvSetNameFlagLong       = "-uvSetName";
const MString MirrorSkinWeightsUV::kHelpFlag                = "-h";
const MString MirrorSkinWeightsUV::kHelpFlagLong            = "-help";
const MString MirrorSkinWeightsUV::kLeftPrefix              = "Left";
const MString MirrorSkinWeightsUV::kRightPrefix             = "Right";

MirrorSkinWeightsUV::MirrorSkinWeightsUV()
    : leftPrefix("Left")
    , rightPrefix("Right")
    , tolerance(0.01)
    , threshold(0.000001)
    , axis(0.5)
    , leftToRight(true)
{
}

MirrorSkinWeightsUV::~MirrorSkinWeightsUV()
{
}

void* MirrorSkinWeightsUV::creator()
{
    return new MirrorSkinWeightsUV;
}


MStatus MirrorSkinWeightsUV::doIt(const MArgList& args)
{
    MStatus status;
    MString meshArg;
    MSyntax syntax;
    syntax.addFlag(kHelpFlag.asChar(), kHelpFlagLong.asChar());
    syntax.addFlag(kMirrorInverseFlag.asChar(), kMirrorInversetFlagLong.asChar(), MSyntax::kBoolean);
    syntax.addFlag(kLeftPrefixFlag.asChar(), kLeftPrefixFlagLong.asChar(), MSyntax::kString);
    syntax.addFlag(kRightPrefixFlag.asChar(), kRightPrefixFlagLong.asChar(), MSyntax::kString);
    syntax.addFlag(kToleranceFlag.asChar(), kToleranceFlagLong.asChar(), MSyntax::kDouble);
    syntax.addFlag(kThresholdFlag.asChar(), kThresholdFlagLong.asChar(), MSyntax::kDouble);
    syntax.addFlag(kExcludeMatchFlag.asChar(), kExcludeMatchFlagLong.asChar(), MSyntax::kString);
    syntax.makeFlagMultiUse(kExcludeMatchFlag.asChar());
    syntax.addFlag(kuvSetNameFlag.asChar(), kuvSetNameFlagLong.asChar(), MSyntax::kString);
    syntax.addFlag(kAxisOnUFlag.asChar(), kAxisOnUFlagLong.asChar(), MSyntax::kDouble);
    syntax.addArg(MSyntax::kString);
    syntax.useSelectionAsDefault(true);
    syntax.setObjectType(MSyntax::kSelectionList, 1, 1);
    syntax.enableEdit(false);
    syntax.enableQuery(false);

    MArgDatabase argData(syntax, args, &status);

    if (argData.isFlagSet(kHelpFlag.asChar()))
    {
        MString helpText;
        helpText += "For each vertex locates the mirrored vertex in UV space copies skinCluster weights to it.\n\n";
        helpText += "Arguments:\n";
        helpText += "\tmesh { str }                         --name of the skinned mesh or (optionally) currently selected mesh.\n";
        helpText += "Keyword Arguments:\n";
        helpText += "\ttolerance(tol) { number }            -- Sets the maximum allowable distance for a matched vertex. (default: 0.01)\n";
        helpText += "\tthreshold(thr) { number }            -- Immediately matches a vertex if less than the specified value. (default: 0.000001)\n";
        helpText += "\taxisOnU(u) { number }                -- Sets the mirror uv axis. (default: 0.5)\n";
        helpText += "\texcludeMatch(exc) { string or list } -- Nodes excluded from mirroring (takes wildcards ie. \"JOFace*\")\n";
        helpText += "\tleftPrefix(lp) { str }               -- Specify left prefix. (Default: \"Left\")\n";
        helpText += "\trightPrefix(rp) { str }              -- Specify right prefix. (Default \"Right\")\n";
        helpText += "\tuvSetName(uvs)  { str }              -- Specify name of uv set. (Default: current uv set)\n";
        helpText += "\tmirrorInverse(mi)  { bool }          -- Values are mirrored from high u to low u. Direction is inverted if true.\n";
        helpText += "\thelp(h) { bool }                     -- Print help.\n";

        displayInfo(helpText);
        return status;
    }


    if (argData.isFlagSet(kMirrorInverseFlag.asChar(), &status)) {
        leftToRight = !argData.flagArgumentBool(kMirrorInverseFlag.asChar(), 0, &status);
    }
	CHECK_MSTATUS(status);


    if (argData.isFlagSet(kToleranceFlag.asChar(), &status))
    {
        tolerance = argData.flagArgumentDouble(kToleranceFlag.asChar(), 0, &status);
    }
	CHECK_MSTATUS(status);

    if (argData.isFlagSet(kThresholdFlag.asChar(), &status))
    {
        threshold = argData.flagArgumentDouble(kThresholdFlag.asChar(), 0, &status);
    }
	CHECK_MSTATUS(status);

    uint numFlagUses = argData.numberOfFlagUses(kExcludeMatchFlag.asChar());
    MArgList argList;
    for (uint i = 0; i < numFlagUses; ++i)
    {
        status = argData.getFlagArgumentList(kExcludeMatchFlag.asChar(), i, argList);
		CHECK_MSTATUS(status);
        MString match = argList.asString(0, &status);
		CHECK_MSTATUS(status);
        match.substitute("*", ".*");
        excludeMatches.append(match);
    }

    if (argData.isFlagSet(kAxisOnUFlag.asChar(), &status))
    {
        axis = argData.flagArgumentDouble(kAxisOnUFlag.asChar(), 0, &status);
    }
	CHECK_MSTATUS(status);

    MSelectionList sList;
    status = argData.getCommandArgument(0, sList);
    if (sList.length() == 0) 
    {
        status = argData.getObjects(sList);
        if (sList.length() == 0) {
            return MS::kNotFound;
        }
    }
	CHECK_MSTATUS(status);

    MDagPath itemDagPath;
    status = sList.getDagPath(0, itemDagPath);
	CHECK_MSTATUS(status);

    meshArg = itemDagPath.partialPathName(&status);
	CHECK_MSTATUS(status);

    MString command;
    MCommandResult commandResult;
    MStringArray resultItems;

    if (argData.isFlagSet(kuvSetNameFlag.asChar(), &status))
    {
        uvCurrentSet = argData.flagArgumentString(kuvSetNameFlag.asChar(), 0, &status);
		CHECK_MSTATUS(status);

        command = "polyUVSet -query -allUVSets \"" + meshArg + "\"";
        status = MGlobal::executeCommand(command, commandResult, false, false);
		CHECK_MSTATUS(status);

        status = commandResult.getResult(resultItems);
		CHECK_MSTATUS(status);

        uint32_t uvSetOccurences = 0;
        for (uint32_t i = 0; i < resultItems.length(); ++i)
        {
            if (uvCurrentSet == resultItems[i]) uvSetOccurences++;
        }
        if (uvSetOccurences == 0)
        {
            uvCurrentSet.clear();
        }

    }
    else {
        command = "polyUVSet -query -currentUVSet \"" + meshArg + "\"";
        status = MGlobal::executeCommand(command, commandResult, false, false);
		CHECK_MSTATUS(status);

        status = commandResult.getResult(resultItems);
		CHECK_MSTATUS(status);

        uvCurrentSet = resultItems[0];
    }

    if (uvCurrentSet.length() == 0)
    {
        MString noUVSetMessage;
        noUVSetMessage = "UV Set not found for mesh " + meshArg + ".";
        MGlobal::displayWarning(noUVSetMessage);
        return status;
    }

    MSelectionList sel;
    status = sel.add(meshArg);
	CHECK_MSTATUS(status);
    status = sel.getDagPath(0, meshDagPath, meshComponent);
	CHECK_MSTATUS(status);

    if (meshComponent.isNull())
    {
        status = meshDagPath.extendToShape();
		CHECK_MSTATUS(status);
    }

    skinCluster = findSkinCluster(meshDagPath);

    if (!skinCluster.isNull())
    {
        redoIt();
    } 
    else
    {
        MString noSkinClusterMessage;
        noSkinClusterMessage = "No skinCluter found for mesh " + meshArg + ".";
        MGlobal::displayWarning(noSkinClusterMessage);
        return status;
    }
    return status;
}

MStatus MirrorSkinWeightsUV::redoIt()
{
    MStatus status;

    MFnSkinCluster skinClusterFn(skinCluster);
    MDagPathArray influenceObjects;
    skinClusterFn.influenceObjects(influenceObjects, &status);
	CHECK_MSTATUS(status);

    MIntArray remap;
    MIntArray excludedIndices;
    indices.clear();

    std::cmatch match;

    MSelectionList sel;
    bool excluded = false;
    for (uint i = 0; i < influenceObjects.length(); i++)
    {
        for (uint j = 0; j < excludeMatches.length(); j++)
        {

            std::regex re(excludeMatches[j].asChar());
            if (std::regex_match(influenceObjects[i].partialPathName().asChar(), match, re))
            {
                status = excludedIndices.append(skinClusterFn.indexForInfluenceObject(influenceObjects[i]));
				CHECK_MSTATUS(status);

                excluded = true;
                break;
            }
        }
        status = sel.clear();
		CHECK_MSTATUS(status);

        status = sel.add(influenceObjects[i]);
		CHECK_MSTATUS(status);

        status = indices.append(skinClusterFn.indexForInfluenceObject(influenceObjects[i]));
		CHECK_MSTATUS(status);

        if (influenceObjects[i].partialPathName().indexW(kLeftPrefix) >= 0)
        {
            MString leftSide = influenceObjects[i].partialPathName();
            status = leftSide.substitute(kLeftPrefix, kRightPrefix);
			CHECK_MSTATUS(status);

            for (uint j = 0; j < influenceObjects.length(); j++)
            {
                MString rightSide = influenceObjects[j].partialPathName();
                if (leftSide == rightSide && !excluded)
                {
                    status = remap.append(skinClusterFn.indexForInfluenceObject(influenceObjects[i]));
					CHECK_MSTATUS(status);

                    status = remap.append(skinClusterFn.indexForInfluenceObject(influenceObjects[j]));
					CHECK_MSTATUS(status);
                }
            }
        }
        excluded = false;
    }

    uint32_t indexA;
    uint32_t indexB;
    for (uint i = 0; i < remap.length(); i += 2)
    {
        indexA = findIndex(remap[i], indices);
        indexB = findIndex(remap[i + 1], indices);
        indices[indexA] = remap[i + 1];
        indices[indexB] = remap[i];
    }

    MIntArray hiIndices;
    MFloatArray hiUCoords;
    MFloatArray hiVCoords;
    MIntArray loIndices;
    MFloatArray loUCoords;
    MFloatArray loVCoords;

    MItMeshVertex meshIter(meshDagPath);

    MComputation computation;
    computation.beginComputation(true, false, true);
    status = computation.setProgressStatus("Sorting UVs");
	CHECK_MSTATUS(status);

    status = computation.setProgressRange(0, meshIter.count());
	CHECK_MSTATUS(status);

    status = computation.setProgress(0);
	CHECK_MSTATUS(status);

    MPoint pt;
    float2 uv;
    while(!meshIter.isDone())
    {
        if (computation.isInterruptRequested())
        {
            displayWarning("Process Interrupted! Exiting.");
            return MS::kSuccess;
        }
        pt = meshIter.position();
        status = meshIter.getUV(uv, &uvCurrentSet);
		CHECK_MSTATUS(status);

        if (sortUV(axis, uv[0], leftToRight))
        {
            status = loIndices.append(meshIter.index());
			CHECK_MSTATUS(status);

            status = loUCoords.append(uv[0]);
			CHECK_MSTATUS(status);

            status = loVCoords.append(uv[1]);
			CHECK_MSTATUS(status);

        } 
        else
        {
            status = hiIndices.append(meshIter.index());
			CHECK_MSTATUS(status);

            status = hiUCoords.append(float(axis) - (uv[0] - float(axis)));
			CHECK_MSTATUS(status);

            status = hiVCoords.append(uv[1]);
			CHECK_MSTATUS(status);
        }

        status = computation.setProgress(meshIter.index());
		CHECK_MSTATUS(status);

        meshIter.next();
    }
    computation.endComputation();

    MFnSingleIndexedComponent sourceComponentsFn;
    MFnSingleIndexedComponent targetComponentsFn;
    MFnSingleIndexedComponent targetComponentsTempFn;
    targetComponents = targetComponentsFn.create(MFn::kMeshVertComponent);
    MDoubleArray newWeights;
    MDoubleArray weights;
    float minDistance;
    float currentDistance;
    int closestComponentIndex;
    int closestComponent;

    computation.beginComputation(true, true, true);
    status = computation.setProgressStatus("Mirroring Weights");
	CHECK_MSTATUS(status);

    status = computation.setProgressRange(0, loIndices.length());
	CHECK_MSTATUS(status);

    status = computation.setProgress(0);
	CHECK_MSTATUS(status);
    uint32_t numExcludedIndices = static_cast<uint32_t>(excludedIndices.length());
    uint32_t vtxCount = 0;
    uint32_t influenceCount = 0;
    for (uint32_t i = 0; i < loIndices.length(); i++) 
    {
        if (computation.isInterruptRequested())
        {
            displayWarning("Process Interrupted! Exiting.");
            return MS::kSuccess;
        }
        minDistance = FLT_MAX;
        closestComponentIndex = -1;
        for (uint j = 0; j < hiIndices.length(); ++j)
        {
            float2 lo;
            lo[0] = loUCoords[i];
            lo[1] = loVCoords[i];

            float2 hi;
            hi[0] = hiUCoords[j];
            hi[1] = hiVCoords[j];

            currentDistance = getDistance(lo, hi);
            if (currentDistance <= tolerance) {
                if (currentDistance <= threshold)
                {
                    closestComponentIndex = j;
                    break;
                }
                else if (currentDistance <= minDistance)
                {
                    minDistance = currentDistance;
                    closestComponentIndex = j;
                }
            }
        }
        if (closestComponentIndex > -1)
        {
            closestComponent = hiIndices[closestComponentIndex];
            hiIndices.remove(closestComponentIndex);
            hiUCoords.remove(closestComponentIndex);
            hiVCoords.remove(closestComponentIndex);

            MObject leftComp = sourceComponentsFn.create(MFn::kMeshVertComponent);
            status = sourceComponentsFn.addElement(closestComponent);
			CHECK_MSTATUS(status);

            MObject rightCompTemp = targetComponentsTempFn.create(MFn::kMeshVertComponent);
            status = targetComponentsTempFn.addElement(loIndices[i]);
			CHECK_MSTATUS(status);

            status = weights.clear();
			CHECK_MSTATUS(status);

            status = skinClusterFn.getWeights(meshDagPath, leftComp, weights, influenceCount);
			CHECK_MSTATUS(status);

            uint32_t numWeights = static_cast<uint32_t>(weights.length());

            MDoubleArray rWeightsTemp;
            status = skinClusterFn.getWeights(meshDagPath, rightCompTemp, rWeightsTemp, influenceCount);
			CHECK_MSTATUS(status);

            double totalWeightExclude = 0.0;

            if (numExcludedIndices > 0) {
                for (uint32_t k = 0; k < numExcludedIndices; ++k)
                {
                    uint excIndex = excludedIndices[k];
                    double wt = rWeightsTemp[excIndex];
                    weights[excIndex] = wt;
                    totalWeightExclude += wt;
                }

                double remainingSum = 1.0 - totalWeightExclude;
                double otherSum = 0.0;
                for (uint32_t k = 0; k < numWeights; ++k)
                {
                    if (findIndex(indices[k], excludedIndices) < 0)
                    {
                        otherSum += weights[k];
                    }
                }
                for (uint32_t k = 0; k < numWeights; ++k)
                {
                    if (findIndex(indices[k], excludedIndices) < 0)
                    {
                        if (otherSum > 0.0)
                        {
                            weights[k] = (weights[k] / otherSum) * remainingSum;
                        } else
                        {
                            weights[k] = rWeightsTemp[k];
                        }
                    }
                }
            }
            for (uint32_t k = 0; k < numWeights; ++k)
            {
                newWeights.append(weights[k]);
            }
            vtxCount += 1;
            status = targetComponentsFn.addElement(loIndices[i]);
			CHECK_MSTATUS(status);
        }
        status = computation.setProgress(i);
		CHECK_MSTATUS(status);
    }
    computation.endComputation();

    status = skinClusterFn.getWeights(meshDagPath, targetComponents, indices, oldWeights);
	CHECK_MSTATUS(status);

    status = skinClusterFn.setWeights(meshDagPath, targetComponents, indices, newWeights);
	CHECK_MSTATUS(status);


    MString vertString;
    vertString += vtxCount;
    vertString += " vertices mirrored";
    displayInfo(vertString);
    return status;
}

MStatus MirrorSkinWeightsUV::undoIt()
{
    MStatus status;
    MFnSkinCluster skinClusterFn(skinCluster);
    status = skinClusterFn.setWeights(meshDagPath, targetComponents, indices, oldWeights);
	CHECK_MSTATUS(status);
    return status;
}

MObject MirrorSkinWeightsUV::findSkinCluster(const MDagPath& skinClusterDagPath) const
{
    MStatus status;
    MObject skinClusterObj;
    MObject geomNode = skinClusterDagPath.node();
    MItDependencyGraph dgIt(geomNode, MFn::kSkinClusterFilter, MItDependencyGraph::kUpstream);
    if (!dgIt.isDone()) {
        skinClusterObj = dgIt.currentItem(&status);
		CHECK_MSTATUS(status);
     }
    return skinClusterObj;
}

int32_t MirrorSkinWeightsUV::findIndex(int indexKey, const MIntArray &influenceIds) const
{
    int32_t numIndices = static_cast<int32_t>(influenceIds.length());
    for (int32_t i = 0; i < numIndices; i++)
    {
        if (indexKey == influenceIds[i])
        {
            return i;
        }
    }
    return -1;
}

float MirrorSkinWeightsUV::getDistance(float2 uvCoordA, float2 uvCoordB) const
{
    return sqrt(((uvCoordA[0] - uvCoordB[0]) * (uvCoordA[0] - uvCoordB[0]) + (uvCoordA[1] - uvCoordB[1]) * (uvCoordA[1] - uvCoordB[1])));
}

bool MirrorSkinWeightsUV::sortUV(double x, double y, bool z) const
{
    if (z)
    {
        return (x > y);
    }
    else
    {
        return (x < y);
    }
}
