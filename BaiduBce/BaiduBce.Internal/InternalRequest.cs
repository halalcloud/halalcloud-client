using System;
using System.Collections.Generic;
using System.IO;

namespace BaiduBce.Internal;

public class InternalRequest
{
	private Stream stream;

	public IDictionary<string, string> Parameters { get; set; }

	public IDictionary<string, string> Headers { get; set; }

	public Uri Uri { get; set; }

	public string HttpMethod { get; set; }

	public Stream Content
	{
		get
		{
			return stream;
		}
		set
		{
			stream = value;
			StartPosition = stream.Position;
		}
	}

	public long StartPosition { get; set; }

	public BceClientConfiguration Config { get; set; }

	public bool Expect100Continue { get; set; }

	public long[] Range { get; set; }

	public InternalRequest()
	{
		Parameters = new Dictionary<string, string>();
		Headers = new Dictionary<string, string>();
	}
}
