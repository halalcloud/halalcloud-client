using System;

namespace BaiduBce.Util.Json;

[AttributeUsage(AttributeTargets.Property)]
public class JsonValueMapper : Attribute
{
	public readonly Type Type;

	public JsonValueMapper(Type type)
	{
		Type = type;
	}
}
