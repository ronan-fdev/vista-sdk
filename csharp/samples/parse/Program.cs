using Vista.SDK;

Console.WriteLine($"=========== Parsing sample - each sample class exposes more methods ===========");
var visVersion = VisVersion.v3_9a;

// GmodPath
var sampleGmodPath = "691.83111i/H101";

// See all static methods on ParseGmodPath for more approaches
var parsedGmodPath = ParseGmodPath.Parse(sampleGmodPath, visVersion);
Console.WriteLine($"Parsed GmodPath: '{sampleGmodPath}' -> '{parsedGmodPath}'");

// Metadata tag
var sampleMetadataTag = "power";
var sampleCodebook = CodebookName.Quantity;
var parsedMetadataTag = ParseMetadataTag.Parse(sampleMetadataTag, sampleCodebook, visVersion);
Console.WriteLine($"Parsed MetadataTag: '{sampleMetadataTag}' -> '{parsedMetadataTag}'");

// LocalId
var sampleLocalId = "/dnv-v2/vis-3-9a/691.83111i/H101/meta/qty-power";

// See all static methods on ParseLocalId for more approaches
var parsedLocalId = ParseLocalId.Parse(sampleLocalId);
Console.WriteLine($"Parsed LocalId: '{sampleLocalId}' -> '{parsedLocalId}'");
