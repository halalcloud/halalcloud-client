using System;

namespace BaiduBce;

public class BceBaseException : Exception
{
	public BceBaseException()
	{
	}

	public BceBaseException(string message)
		: base(message)
	{
	}

	public BceBaseException(string message, Exception innerException)
		: base(message, innerException)
	{
	}
}
