using Vista.SDK;

Console.WriteLine($"=========== Versioning sample - Expects 691.81 -> 691.8311 ===========");
var sourceVisVersion = VisVersion.v3_7a;
var targetVisVersion = VisVersion.v3_9a;

// GmodNode
var sampleGmodNode = "691.81";
var convertedGmod = ConvertGmodNode.Convert(
    gmodNode: sampleGmodNode,
    sourceVersion: sourceVisVersion,
    targetVersion: targetVisVersion
);
Console.WriteLine($"Converted GmodNode: Source='{sampleGmodNode}' Target='{convertedGmod}'");

// GmodPath
var sampleGmodPath = "691.811i/H101";
var convertedGmodPath = ConvertGmodPath.Convert(
    gmodPath: sampleGmodPath,
    sourceVersion: sourceVisVersion,
    targetVersion: targetVisVersion
);
Console.WriteLine($"Converted GmodPath: Source='{sampleGmodPath}' Target='{convertedGmodPath}'");

// LocalId
var sampleLocalId = "/dnv-v2/vis-3-7a/691.811i/H101/meta/qty-power";
var convertedLocalId = ConvertLocalId.Convert(localId: sampleLocalId, targetVersion: targetVisVersion);
Console.WriteLine($"Converted LocalId: Source='{sampleLocalId}' Target='{convertedLocalId}'");
