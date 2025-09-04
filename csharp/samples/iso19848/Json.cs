using System.Diagnostics;
using Vista.SDK.Transport.Json;
using Vista.SDK.Transport.Json.DataChannel;
using Vista.SDK.Transport.Json.TimeSeriesData;

static class JsonToDomain
{
    public static Vista.SDK.Transport.DataChannel.DataChannelListPackage DeserializeDataChannelList()
    {
        var file = File.ReadAllText("resources/DataChannelList.json");
        // SDK provides a configured JSON serializer
        var json = Serializer.DeserializeDataChannelList(file);
        Debug.Assert(json != null);
        return json.ToDomainModel();
    }

    public static Vista.SDK.Transport.TimeSeries.TimeSeriesDataPackage DeserializeTimeSeriesDataPackage()
    {
        var file = File.ReadAllText("resources/TimeSeriesDataPackage.json");
        // SDK provides a configured JSON serializer
        var json = Serializer.DeserializeTimeSeriesData(file);
        Debug.Assert(json != null);
        return json.ToDomainModel();
    }

    public static string Serialize(Vista.SDK.Transport.DataChannel.DataChannelListPackage dataChannelListPackage)
    {
        // SDK provides a configured JSON serializer
        return Serializer.Serialize(dataChannelListPackage.ToJsonDto());
    }

    // SDK provides a configured JSON serializer
    public static string Serialize(Vista.SDK.Transport.TimeSeries.TimeSeriesDataPackage timeSeriesDataPackage) =>
        Serializer.Serialize(timeSeriesDataPackage.ToJsonDto());
}
