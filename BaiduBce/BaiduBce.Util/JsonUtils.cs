using System;
using System.IO;
using Newtonsoft.Json;
using Newtonsoft.Json.Serialization;

namespace BaiduBce.Util;

public static class JsonUtils
{
	private static JsonSerializer serializer = new JsonSerializer
	{
		NullValueHandling = (NullValueHandling)1,
		DateFormatHandling = (DateFormatHandling)0,
		MissingMemberHandling = (MissingMemberHandling)0
	};

	private static JsonSerializerSettings jsonSerializerSettings = new JsonSerializerSettings
	{
		ContractResolver = (IContractResolver)new CamelCasePropertyNamesContractResolver()
	};

	public static T ToObject<T>(StreamReader input)
	{
		//IL_0001: Unknown result type (might be due to invalid IL or missing references)
		//IL_0007: Expected O, but got Unknown
		JsonReader val = (JsonReader)new JsonTextReader((TextReader)input);
		try
		{
			return serializer.Deserialize<T>(val);
		}
		finally
		{
			((IDisposable)val)?.Dispose();
		}
	}

	public static string SerializeObject(object value)
	{
		return JsonConvert.SerializeObject(value, jsonSerializerSettings);
	}
}
