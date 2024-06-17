using System;
using System.IO;
using System.Net;
using System.Text;
using BaiduBce.Http;
using BaiduBce.Internal;
using BaiduBce.Model;
using BaiduBce.Util;

namespace BaiduBce;

public abstract class BceClientBase
{
	private string serviceEndpointFormat;

	protected BceClientConfiguration config;

	internal BceHttpClient httpClient;

	public BceClientBase(BceClientConfiguration config, string serviceEndpointFormat)
	{
		this.serviceEndpointFormat = serviceEndpointFormat;
		this.config = BceClientConfiguration.CreateWithDefaultValues().Merge(config);
		httpClient = new BceHttpClient();
	}

	public string ComputeEndpoint(BceClientConfiguration config)
	{
		if (config.Endpoint != null)
		{
			if (!config.Endpoint.Contains("://"))
			{
				return "http://" + config.Endpoint;
			}
			return config.Endpoint;
		}
		string arg = config.Protocol ?? "http";
		string arg2 = config.Region ?? "bj";
		return string.Format(serviceEndpointFormat, arg, arg2);
	}

	protected InternalRequest CreateInternalRequest(BceRequestBase request, string httpMethod, string[] pathComponents)
	{
		InternalRequest internalRequest = new InternalRequest();
		internalRequest.Config = config.Merge(request.Config);
		if (request.Credentials != null)
		{
			internalRequest.Config.Credentials = request.Credentials;
		}
		internalRequest.Uri = new Uri(HttpUtils.AppendUri(ComputeEndpoint(internalRequest.Config), pathComponents));
		internalRequest.HttpMethod = httpMethod;
		DateTime dateTime = internalRequest.Config.SignOptions.Timestamp;
		if (dateTime == DateTime.MinValue)
		{
			dateTime = DateTime.Now;
		}
		internalRequest.Headers["x-bce-date"] = DateUtils.FormatAlternateIso8601Date(dateTime);
		internalRequest.Headers["Host"] = HttpUtils.GenerateHostHeader(internalRequest.Uri);
		return internalRequest;
	}

	protected void FillRequestBodyForJson(InternalRequest internalRequest, string json)
	{
		byte[] bytes = Encoding.UTF8.GetBytes(json);
		internalRequest.Headers["Content-Length"] = bytes.Length.ToString();
		internalRequest.Headers["Content-Type"] = "application/json; charset=utf-8";
		internalRequest.Content = new MemoryStream(bytes);
	}

	protected virtual T ToObject<T>(HttpWebResponse httpWebResponse) where T : BceResponseBase, new()
	{
		Stream responseStream = httpWebResponse.GetResponseStream();
		if (responseStream != null)
		{
			T val = JsonUtils.ToObject<T>(new StreamReader(responseStream));
			if (val == null)
			{
				val = new T();
			}
			val.BceRequestId = httpWebResponse.Headers["x-bce-request-id"];
			return val;
		}
		return null;
	}

	protected void CheckNotNull(object obj, string message)
	{
		if (obj == null)
		{
			throw new ArgumentNullException(message);
		}
	}

	protected void CheckNotNullOrEmpty(string str, string message)
	{
		if (string.IsNullOrEmpty(str))
		{
			throw new ArgumentNullException(message);
		}
	}
}
