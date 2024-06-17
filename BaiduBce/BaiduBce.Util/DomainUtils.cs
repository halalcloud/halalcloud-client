using System;
using System.Collections.Generic;

namespace BaiduBce.Util;

public class DomainUtils
{
	private static readonly List<string> DefaultCnameLikeList = new List<string> { ".cdn.bcebos.com" };

	private const string HttpProtocolHead = "http";

	private const string HttpsProtocolHead = "https";

	public static bool IsCnameLikeHost(string host)
	{
		if (string.IsNullOrEmpty(host))
		{
			return false;
		}
		host = host.ToLower();
		foreach (string defaultCnameLike in DefaultCnameLikeList)
		{
			if (host.EndsWith(defaultCnameLike))
			{
				return true;
			}
		}
		return false;
	}

	public static bool IsCustomHost(string host, string bucketName)
	{
		if (string.IsNullOrEmpty(host) || string.IsNullOrEmpty(bucketName))
		{
			return false;
		}
		host = host.ToLower();
		if (host.StartsWith("http") || host.StartsWith("https"))
		{
			string[] array = host.Split(new string[1] { "//" }, StringSplitOptions.None);
			if (array.Length == 2)
			{
				return array[1].StartsWith(bucketName);
			}
			return false;
		}
		return host.StartsWith(bucketName);
	}

	public static string ConvertEndpointToVirtualHostedStyle(string host, string bucketName)
	{
		if (string.IsNullOrEmpty(host) || string.IsNullOrEmpty(bucketName))
		{
			return host;
		}
		host = host.ToLower();
		if (host.StartsWith("http") || host.StartsWith("https"))
		{
			string[] array = host.Split(new string[1] { "//" }, StringSplitOptions.None);
			if (array.Length == 2)
			{
				return array[0] + "//" + ConvertWithoutHead(array[1], bucketName);
			}
			return host;
		}
		return ConvertWithoutHead(host, bucketName);
	}

	private static string ConvertWithoutHead(string host, string bucketName)
	{
		string[] array = host.Split(new string[1] { "." }, StringSplitOptions.None);
		if (array.Length == 3)
		{
			string[] array2 = new string[array.Length + 1];
			array2[0] = bucketName;
			Array.Copy(array, 0, array2, 1, array.Length);
			return string.Join(".", array2);
		}
		return host;
	}
}
