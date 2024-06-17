using System;

namespace BaiduBce;

public class BceClientException : BceBaseException
{
	public BceClientException()
	{
	}

	public BceClientException(string message)
		: base(message)
	{
	}

	public BceClientException(string message, Exception innerException)
		: base(message, innerException)
	{
	}
}
