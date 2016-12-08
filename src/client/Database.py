import os
import shelve


class Database(object):
    def __init__(self, name):
        """
        Creates a database structure of nested objects.

                Doctest:
                    >>> db = Database('test')
                    """
        self.name = name
        self.tables = dict()
        self.count = 0

    def add_table(self, name, fields):
        """
        Create a new table object within the database.

            Doctest:
                >>> db = Database('test')
                >>> db.add_table('test', ['one'])
                >>> print(db.tables['test'].fields)
                ['one']
                """
        self.tables[name] = self.Table(name, fields)

    def drop_table(self, name):
        """
        Remove table object [name] from the database.

            Doctest:
                >>> db = Database('test')
                >>> db.add_table('test', ['one'])
                >>> print(db.tables['test'].fields)
                ['one']
                >>> db.drop_table('test')
                >>> print(db.tables)
                {}
                """
        self.tables.pop(name)

    def insert_row(self, table, name, values):
        """
        Insert row [name] with fields [where field is List] into [table]

            Doctest:
                >>> db = Database('test')
                >>> db.add_table('test', ['one'])
                >>> db.insert_row('test', 'test', ['one'])
                >>> print(db.tables['test'].rows['test'].stuff)
                {'one': 'one'}
                """
        self.tables[table].add_row(
            name, self.count, self.tables[table].fields, values)
        self.count += 1

    def drop_row(self, name, row):
        """
        Delete [row] from table [name]

            Doctest:
                >>> db = Database('test')
                >>> db.add_table('test', ['one'])
                >>> db.insert_row('test', 'test', ['one'])
                >>> print(db.tables['test'].rows['test'].stuff)
                {'one': 'one'}
                >>> db.drop_row('test', 'test')
                >>> print(db.tables['test'].rows)
                {}
                """
        self.tables[name].rows.pop(row)

    def get_field_by_name(self, field, table, name):
        """
        Get [field] from row [name] in [table]

            Doctest:
                >>> db = Database('test')
                >>> db.add_table('test', ['one'])
                >>> db.insert_row('test', 'test', ['one'])
                >>> print(db.tables['test'].rows['test'].stuff)
                {'one': 'one'}
                >>> db.get_field_by_name('one', 'test', 'test')
                'one'
                """
        return self.tables[table].rows[name].stuff[field]

    def get_field_by_condition(self, x, y, z, table):
        """
        Get key [z] from [table] if value [y] is in key [x]

            Doctest:
                >>> db = Database('test')
                >>> db.add_table('test', ['one'])
                >>> db.insert_row('test', 'test', ['one'])
                >>> print(db.tables['test'].rows['test'].stuff)
                {'one': 'one'}
                >>> db.get_field_by_condition('one', 'one', 'one', 'test')
                'one'
                """
        for a in self.tables[table].rows:
            if self.tables[table].rows[a].stuff[x] == y:
                return self.tables[table].rows[a].stuff[z]

    def get_field_by_id(self, field, table, id):
        """
        Get [field] from row [id] in [table]

            Doctest:
                >>> db = Database('test')
                >>> db.add_table('test', ['one'])
                >>> db.insert_row('test', 'test', ['one'])
                >>> print(db.tables['test'].rows['test'].stuff)
                {'one': 'one'}
                >>> db.get_field_by_id('one', 'test', 0)
                'one'
                """
        for a in self.tables[table].rows:
            if self.tables[table].rows[a].id == id:
                return self.tables[table].rows[a].stuff[field]

    def commit(self):
        """
        Get [field] from row [ID] in [table]

            Doctest:
                >>> db = Database('test')
                >>> db.add_table('test', ['one'])
                >>> db.insert_row('test', 'test', ['one'])
                >>> db.commit()
                """
        shelf = shelve.open(self.name)
        shelf['db'] = self
        shelf.close()

    def db_close(self):
        """
        Doctest:
        >>> db = Database('test')
        >>> db.db_close()
        """
        self.commit()

    class Table(object):
        def __init__(self, name, fields):
            self.name = name
            self.fields = fields
            self.rows = dict()

        def add_row(self, name, count, fields, values):
            self.rows[name] = self.Row(name, count, fields, values)

        class Row(object):
            def __init__(self, name, id, fields, values):
                self.name = name
                self.id = id
                self.stuff = dict()
                for x in range(len(fields)):
                    self.stuff[fields[x]] = values[x]


def db_open(name):
    if not os.path.exists('test.dat'):
        return Database(name)
    else:
        shelf = shelve.open('test')
        tmp = shelf['db']
        shelf.close()
        return tmp
