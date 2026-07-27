# Notaryze MCP

`notaryze_mcp.py` is a small stdio MCP server for local macOS signing checks.
It has no third-party Python dependencies.

Run it with:

```sh
python3 tools/notaryze_mcp.py
```

Available tools:

- `list_signing_identities`
- `verify_signature`
- `sign_ad_hoc`
- `remove_signature`
- `sign_developer_id`
- `create_sample_executable`
