using System;

namespace BaiduBce.Services.Bos.Model;

public class ObjectRequestBase : BucketRequestBase
{
	private const int MaxObjectKeyLength = 1024;

	private string _key;

	private long _trafficLimit;

	public string Key
	{
		get
		{
			return _key;
		}
		set
		{
			if (string.IsNullOrEmpty(value))
			{
				throw new ArgumentNullException("object key should not be null or empty");
			}
			if (value.Length > 1024)
			{
				throw new ArgumentException("objectKey should not be greater than " + 1024 + ".");
			}
			_key = value;
		}
	}

	public long TrafficLimit
	{
		get
		{
			return _trafficLimit;
		}
		set
		{
			if (value == 0L || (value >= 819200 && value <= 838860800))
			{
				_trafficLimit = value;
				return;
			}
			throw new ArgumentOutOfRangeException("Traffic limit must be between 819200(bit/s) and 838860800(bit/s).");
		}
	}
}
