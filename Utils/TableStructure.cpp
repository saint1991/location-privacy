#include "stdafx.h"
#include "TableStructure.h"


///<summary>
/// �R���X�g���N�^
///</summary>
Db::TableStructure::TableStructure(std::string table_name, std::list<Db::Column> columns) 
	: table_name(table_name), columns(std::make_unique<std::list<Db::Column>>(columns))
{
}


///<summary>
/// �R�s�[�R���X�g���N�^
///</summary>
Db::TableStructure::TableStructure(const Db::TableStructure& structure) : table_name(structure.table_name), columns(std::make_unique<std::list<Db::Column>>())
{
	for (std::list<Db::Column>::const_iterator iter = structure.columns->begin(); iter != structure.columns->end(); iter++) {
		columns->push_back(*iter);
	}
	if (structure.primary_keys != nullptr) primary_keys = std::make_unique<std::list<std::string>>();
	for (std::list<std::string>::const_iterator iter = structure.primary_keys->begin(); iter != structure.primary_keys->end(); iter++) {
		primary_keys->push_back(*iter);
	}
}

///<summary>
/// �f�X�g���N�^
///</summary>
Db::TableStructure::~TableStructure()
{
}


///<summary>
/// �w�肳�ꂽ�J�������̃J���������݂���ꍇtrue��Ԃ��܂�
///</summary>
bool Db::TableStructure::is_column_exists(const std::string& column_name) const
{
	auto iter = std::find_if(columns->begin(), columns->end(), [column_name](Db::Column column) -> bool {
		return column_name == column.column_name;
	});
	return iter != columns->end();
	return false;
}


///<summary>
/// �J�����̃C���f�b�N�X���擾����
/// ������Ȃ��ꍇ��0��Ԃ��D
/// INSERT���ł̃o�C���h���Ɏg�p�ł���
///</summary>
const unsigned int Db::TableStructure::get_column_index(const std::string& column_name) const
{
	int index = 1;
	for (std::list<Db::Column>::const_iterator iter = columns->begin(); iter != columns->end(); iter++) {
		if (iter->column_name == column_name) break;
		index++;
	}
	return index > columns->size() ? 0 : index;
}



///<summary>
/// �J�������̈ꗗ���擾����
///</summary>
const std::list<std::string> Db::TableStructure::get_column_name_list() const
{
	std::list<std::string> column_names;
	for (std::list<Db::Column>::const_iterator iter = columns->begin(); iter != columns->end(); iter++) {
		column_names.push_back(iter->column_name);
	}
	return column_names;
}


///<summary>
/// PRIMARY KEY�̐ݒ��ǉ����܂�
/// ���݂��Ȃ��J�������w�肵���ꍇ��false��Ԃ��܂��D
///</summary>
bool Db::TableStructure::add_primary_key(std::string primary_key)
{
	if (is_column_exists(primary_key)) {
		if (primary_keys == nullptr) primary_keys = std::make_unique<std::list<std::string>>();
		primary_keys->push_back(primary_key);
		return true;
	}
	return false;
}
