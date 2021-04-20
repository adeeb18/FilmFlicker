import pandas as pd
# Basic scripts to turn tsv to csv (Commented out lines involve removing non movies for that file)
table = pd.read_csv('data_3.tsv', sep="\t")

# index_names = table[(table['titleType'] != 'movie')].index


#table.drop(index_names, inplace = True)

#print(table.head())


table.to_csv('Supp_Cast.csv')