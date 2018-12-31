{
	'targets': [
		{
			'target_name': 'isolated-native-example',
			'include_dirs': [
				'<!(node -e "require(\'nan\')")',
				'<!(node -e "require(\'catbox-v8/include\')")',
			],
			'sources': [
				'example.cc',
			],
		},
	],
}
