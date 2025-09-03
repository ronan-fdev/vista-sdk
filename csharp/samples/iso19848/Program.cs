using System.Globalization;
using Vista.SDK;
using Vista.SDK.Transport.Json;
using Vista.SDK.Transport.Json.DataChannel;
using Vista.SDK.Transport.Json.TimeSeriesData;

Console.WriteLine($"=========== Read ISO 19848 JSON to domain example ===========");
var dataChannelListPackage = JsonToDomain.ReadDataChannelList();
var timeSeriesDataPackage = JsonToDomain.ReadTimeSeriesDataPackage();
Console.WriteLine($"Read DataChannelList: \n{dataChannelListPackage.ToJsonDto().Serialize()}");
Console.WriteLine($"Read TimeSeriesDataPackage: \n{timeSeriesDataPackage.ToJsonDto().Serialize()}");

Console.WriteLine($"=========== ISO 19848 Domain example ===========");
var shipId = "IMO1234567";

// This would typically be stored in the DataChannelList JSON file, as shown above, but for the sake of the example - we will define it here
var map = new Dictionary<string, LocalId>()
{
    { "Speed", LocalId.Parse("/dnv-v2/vis-3-9a/411.1/C101.663i/C663/meta/qty-rotational.frequency") },
    {
        "LubeOilPressureInlet",
        LocalId.Parse("/dnv-v2/vis-3-9a/411.1/C101.663i/C663/meta/qty-pressure/cnt-lubricating.oil/pos-inlet")
    },
    {
        "LubeOilTemperatureInlet",
        LocalId.Parse("/dnv-v2/vis-3-9a/411.1/C101.663i/C663/meta/qty-temperature/cnt-exhaust.gas/pos-inlet")
    },
    {
        "LubeOilTemperatureOutlet",
        LocalId.Parse("/dnv-v2/vis-3-9a/411.1/C101.663i/C663/meta/qty-temperature/cnt-lubricating.oil/pos-outlet")
    },
    {
        "ExhaustGasTemperatureOutlet",
        LocalId.Parse("/dnv-v2/vis-3-9a/411.1/C101.663i/C663/meta/qty-temperature/cnt-exhaust.gas/pos-outlet")
    },
    { "Latitude", LocalId.Parse("/dnv-v2/vis-3-9a/710.1/F211.11/meta/qty-latitude") },
    { "Longitude", LocalId.Parse("/dnv-v2/vis-3-9a/710.1/F211.11/meta/qty-longitude") }
};

var dataChannelList = DataChannelList.Create(shipId, map);
var dcJson = dataChannelList.ToJsonDto().Serialize();
Console.WriteLine($"Produced DataChannelList JSON:\n{dcJson}");

var sourceData = new[]
{
    new TurboChargerData(
        Timestamp: DateTimeOffset.Parse("2023-09-13T21:04:38.00Z", CultureInfo.InvariantCulture),
        Speed: 15000,
        LubeOilPressureInlet: 3.5,
        LubeOilTemperatureInlet: 90,
        LubeOilTemperatureOutlet: 85,
        ExhaustGasTemperatureOutlet: 550,
        Position: (62.84933, 7.120334)
    )
};

var package = TimeSeries.GetPackage(shipId, dataChannelList, sourceData);

// Serialize it
var json = package.ToJsonDto().Serialize();

Console.WriteLine($"Produced TimeSeriesDataPackage:\n{json}");
