 #ifndef CHCOOMATRIX_H
 #define CHCOOMATRIX_H

#include "ChApiMumps.h"
#include "chrono/core/ChMatrix.h"
#include "chrono/core/ChSparseMatrix.h"
#include <vector>
#include <core/ChMapMatrix.h>
#include <core/ChCSR3Matrix.h>
#include <core/ChTimer.h>

namespace chrono{

    class ChApiMumps ChCOOMatrix : public ChCSR3Matrix
    {

    private:
        std::vector<int> rowIndex;
        std::vector<int> colIndex;
        std::vector<double> values;
        bool one_indexed = true;

    public:
        explicit ChCOOMatrix(int mat_rows = 3, int mat_cols = 3, bool one_indexed = true) :
            ChCSR3Matrix(mat_rows, mat_cols), one_indexed(one_indexed) {}

        virtual ~ChCOOMatrix() {}

        void SetOneIndexing(bool val) { one_indexed = val; }

        /// Return the row index array in the COO representation of this matrix.
        int* GetCOO_RowIndexAddress() const
        {
            auto der_ptr = const_cast<ChCOOMatrix*>(this);
            auto base_ptr = static_cast<ChCSR3Matrix*>(der_ptr);

            auto array_length = GetTrailingIndexLength();
            der_ptr->rowIndex.resize(array_length);
            auto one_index_offset = one_indexed ? 1 : 0;

            if (!base_ptr->IsRowMajor())
            {
                // in this case, the underlaying CSR3 matrix has a full-length row index array
                // that has just to be eventually adapted to comply with the one-indexed array format
                auto row_array = base_ptr->GetCSR_TrailingIndexArray();;

                if (!one_indexed)
                    return row_array;

                for (auto el_sel = 0; el_sel < array_length; ++el_sel)
                {
                    der_ptr->rowIndex[el_sel] = row_array[el_sel] + one_index_offset;
                }

            }
            else
            {
                // in this case, the underlaying CSR3 matrix has a compressed row index array
                // so a full-length array has to be created, expanding the CSR one
                auto row_array = base_ptr->GetCSR_LeadingIndexArray();;
                for (auto row_sel = 0; row_sel<GetNumRows(); ++row_sel)
                {
                    for (auto el_sel = row_array[row_sel]; el_sel < row_array[row_sel + 1]; ++el_sel)
                    {
                        der_ptr->rowIndex[el_sel] = row_sel + one_index_offset;
                    }
                }
            }

            return const_cast<int*>(rowIndex.data());
        }


        /// Return the column index array in the COO representation of this matrix.
        int* GetCOO_ColIndexAddress() const {
            auto der_ptr = const_cast<ChCOOMatrix*>(this);
            auto base_ptr = static_cast<ChCSR3Matrix*>(der_ptr);

            auto array_length = GetTrailingIndexLength();
            der_ptr->colIndex.resize(array_length);
            auto one_index_offset = one_indexed ? 1 : 0;

            if (base_ptr->IsRowMajor())
            {
                // in this case, the underlaying CSR3 matrix has a full-length col index array
                // that has just to be eventually adapted to comply with the one-indexed array format
                auto col_array = base_ptr->GetCSR_TrailingIndexArray();

                if (!one_indexed)
                    return col_array;

                for (auto el_sel = 0; el_sel < array_length; ++el_sel)
                {
                    der_ptr->colIndex[el_sel] = col_array[el_sel] + one_index_offset;
                }

            }
            else
            {
                // in this case, the underlaying CSR3 matrix has a compressed col index array
                // so a full-length array has to be created, expanding the CSR one
                auto col_array = base_ptr->GetCSR_LeadingIndexArray();

                for (auto col_sel = 0; col_sel<GetNumRows(); ++col_sel)
                {
                    for (auto el_sel = col_array[col_sel]; el_sel < col_array[col_sel + 1]; ++el_sel)
                    {
                        der_ptr->colIndex[el_sel] = col_sel + one_index_offset;
                    }
                }
            }

            return const_cast<int*>(colIndex.data());
        }


        /// Return the array of matrix values in the CSR representation of this matrix.
        double* GetCOO_ValuesAddress() const { return static_cast<ChCSR3Matrix*>(const_cast<ChCOOMatrix*>(this))->GetCSR_ValueArray(); }

    };

}

#endif