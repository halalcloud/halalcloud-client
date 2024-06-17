using System;

namespace BaiduBce.Util.Json;

[AttributeUsage(AttributeTargets.Property)]
public class JsonField : Attribute
{
	public readonly string Name;

	public JsonField(string name)
	{
		Name = name;
	}
}
