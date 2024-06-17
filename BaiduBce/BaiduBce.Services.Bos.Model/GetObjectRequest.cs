using System;

namespace BaiduBce.Services.Bos.Model;

public class GetObjectRequest : ObjectRequestBase
{
	public long[] Range { get; set; }

	public void SetRange(long start, long end)
	{
		if (start < 0)
		{
			throw new ArgumentException("start should be non-negative");
		}
		if (start > end)
		{
			throw new ArgumentException("start should not be greater than end");
		}
		Range = new long[2] { start, end };
	}
}
