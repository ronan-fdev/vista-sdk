using System.Diagnostics;
using Vista.SDK;

Console.WriteLine($"=========== Builder sample - LocalId ===========");

// Creating new - "/dnv-v2/vis-3-9a/691.83111i/H101/meta/qty-power"
var newLocalId = BuildLocalId.BuildNew(
    visVersion: VisVersion.v3_9a,
    // For the sake of this example - only primaryItem and quantity is configured
    primaryItem: "691.83111i/H101",
    quantity: "power"
);
Console.WriteLine($"Built new LocalId: '{newLocalId}'");

// From existing
var fromExisting = BuildLocalId.BuildFromExisting("/dnv-v2/vis-3-9a/691.83111i/H101/meta/qty-power");
Console.WriteLine($"Built LocalId from existing: '{fromExisting}'");

// Verbose mode
var verbose = BuildLocalId.BuildVerbose(fromExisting);
Console.WriteLine($"Built LocalId (verbose): '{verbose}'");

// Remove properties - always null due to invalidity
var builderWithoutProperties = BuildLocalId.RemoveProperties(fromExisting);
Console.WriteLine($"Built LocalId (without properties): '{builderWithoutProperties?.ToString() ?? "Not valid"}'");

Console.WriteLine($"\n\n=========== Builder sample - LocalIdQuery ===========");
var sampleLocalId = LocalId.Parse("/dnv-v2/vis-3-7a/691.811i-A/H101.11-1/meta/qty-power");
var convertedSampleLocalId = VIS.Instance.ConvertLocalId(sampleLocalId, VisVersion.v3_9a);
Debug.Assert(convertedSampleLocalId != null);
var randomOtherLocalId = LocalId.Parse("/dnv-v2/vis-3-9a/411.1-1/C101/meta/cnt-lubricating.oil");

// Match self
var result = BuildLocalIdQuery.MatchSelf(sampleLocalId);
Console.WriteLine($"MatchSelf: LocalId='{sampleLocalId}' Match='{result}' Expects='True'");

// Match all
result = BuildLocalIdQuery.MatchAll(sampleLocalId);
Console.WriteLine($"MatchAll: LocalId='{sampleLocalId}' Match='{result}' Expects='True'");
result = BuildLocalIdQuery.MatchAll(randomOtherLocalId);
Console.WriteLine($"MatchAll: LocalId='{randomOtherLocalId}' Match='{result}' Expects='True'");

// Match across VIS versions
result = BuildLocalIdQuery.MatchAcrossVisVersions(sampleLocalId, convertedSampleLocalId);
Console.WriteLine(
    $"MatchAcrossVisVersions: Source='{sampleLocalId}' Target='{convertedSampleLocalId}' Match='{result}' Expects='True'"
);

// Match metadata tags
result = BuildLocalIdQuery.MatchMetadataTag(CodebookName.Quantity, "power", sampleLocalId);
Console.WriteLine($"MatchMetadataTag: LocalId='{sampleLocalId}' Match='{result}' Expects='True'");
result = BuildLocalIdQuery.MatchMetadataTag(CodebookName.Quantity, "power", randomOtherLocalId);
Console.WriteLine($"MatchMetadataTag: LocalId='{randomOtherLocalId}' Match='{result}' Expects='False'");

// Match all instances of a GmodPath
var pathMatchLocalId = sampleLocalId
    .Builder
    .WithPrimaryItem(GmodPath.Parse("691.811i-F/H101.11-2", sampleLocalId.VisVersion)) // Note - changed location of 691.811i
    .Build();
result = BuildLocalIdQuery.MatchAllInstancesOfAGmodPath(sampleLocalId, pathMatchLocalId);
Console.WriteLine(
    $"MatchAllInstancesOfAGmodPath: Source='{sampleLocalId}' Target='{pathMatchLocalId}' Match='{result}' Expects='True'"
);

// Match all instances of a GmodNode in a GmodPath - Expects false because H101.11-2 is not configured to "match all"
result = BuildLocalIdQuery.MatchAllInstancesOfAGmodNodeInGmodPath(sampleLocalId, "691.811i", pathMatchLocalId);
Console.WriteLine(
    $"MatchAllInstancesOfAGmodNodeInGmodPath: Source='{sampleLocalId}' Target='{pathMatchLocalId}' Match='{result}' Expects='False'"
);
