{
	'targets': [
		{
			'include_dirs': [
				"<!(node -e \"require('nan')\")"
			],
			'target_name': 'winfileversion',
			'sources': [ 'WinFileVersion.cc' ]
		}
	]
}