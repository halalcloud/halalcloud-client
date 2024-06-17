using System;

namespace BaiduBce.Util.Json.Example;

public class Book
{
	public Author Author { get; set; }

	public string Title { get; set; }

	public double Price { get; set; }

	[JsonField("date")]
	[JsonValueMapper(typeof(DateMapper))]
	public DateTime PublishDate { get; set; }
}
