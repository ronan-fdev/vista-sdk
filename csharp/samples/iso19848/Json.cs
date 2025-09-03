using System.Diagnostics;
using Vista.SDK.Transport.Json;
using Vista.SDK.Transport.Json.DataChannel;
using Vista.SDK.Transport.Json.TimeSeriesData;

static class JsonToDomain
{
    public static Vista.SDK.Transport.DataChannel.DataChannelListPackage ReadDataChannelList()
    {
        var file = File.ReadAllText("resources/DataChannelList.json");
        var json = Serializer.DeserializeDataChannelList(file);
        Debug.Assert(json != null);
        return json.ToDomainModel();
    }

    public static Vista.SDK.Transport.TimeSeries.TimeSeriesDataPackage ReadTimeSeriesDataPackage()
    {
        var file = File.ReadAllText("resources/TimeSeriesDataPackage.json");
        var json = Serializer.DeserializeTimeSeriesData(file);
        Debug.Assert(json != null);
        return json.ToDomainModel();
    }
}
