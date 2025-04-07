import pandas as pd
from sklearn.ensemble import RandomForestRegressor
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import PolynomialFeatures
from sklearn.model_selection import train_test_split
from sklearn.metrics import r2_score
from sklearn.preprocessing import PolynomialFeatures
import matplotlib.pyplot as plt
import statsmodels.formula.api as smf
import seaborn as sns

data = pd.read_csv("data/data.csv")

# Step 2: Split data into features (X) and target (y)
# Assuming 'target' is the name of the column you're trying to predict
X = data.drop('Result', axis=1)  # Drop the target column
y = data['Result']  # This is your dependent variable

poly = PolynomialFeatures(degree=2, interaction_only=True, include_bias=False)
X_interactions = poly.fit_transform(X)

# Convert the transformed features back into a DataFrame
interaction_columns = poly.get_feature_names_out(X.columns)
X_interactions_df = pd.DataFrame(X_interactions, columns=interaction_columns)

# Step 4: Split the data into training and test sets
X_train, X_test, y_train, y_test = train_test_split(X_interactions_df, y, test_size=0.2, random_state=42)

# Step 5: Train a Random Forest model with the interaction terms
rf = RandomForestRegressor(n_estimators=100, random_state=42)
rf.fit(X_train, y_train)

# Step 6: Get feature importances for the interaction terms
importances = rf.feature_importances_

# Step 7: Display feature importances (including interaction terms)
for feature, importance in zip(X_interactions_df.columns, importances):
    print(f"{feature}: {importance}")

r2_train = rf.score(X_train, y_train)  # R² on the training data
r2_test = rf.score(X_test, y_test)  # R² on the test data

importances = rf.feature_importances_
features = X_interactions_df.columns

# Create a DataFrame for easier plotting
importance_df = pd.DataFrame({'Feature': features, 'Importance': importances})
importance_df = importance_df.sort_values(by='Importance', ascending=False)

# Plot
plt.figure(figsize=(10, 6))
sns.barplot(x='Importance', y='Feature', data=importance_df)
plt.title('Feature Importance from Random Forest Model')
plt.savefig("feature_importance.png")


# Print the R² values
print(f"R² on Training Data: {r2_train}")
print(f"R² on Test Data: {r2_test}")


model = smf.ols(formula='Result ~ X1 * X2 * X3', data=data).fit()

summary_text = model.summary().as_text()

# Create figure
fig, ax = plt.subplots(figsize=(12, 8))
ax.axis('off')  # No axes
ax.text(0, 1, summary_text, fontsize=10, family='monospace', va='top')

# Save to file
plt.savefig('regression_summary.png', bbox_inches='tight')
