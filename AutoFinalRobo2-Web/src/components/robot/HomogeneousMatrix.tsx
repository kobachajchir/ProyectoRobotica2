import type { Matrix4 } from 'three';

function cell(value: number, row: number, col: number) {
  if (row === 3) return String(Math.round(value));
  const scaled = col === 3 ? value * 1000 : value;
  if (Math.abs(scaled) < (col === 3 ? .05 : .0005)) return '0';
  return scaled.toFixed(col === 3 ? 1 : 3);
}

/** One matrix drawing for every simulation. Input translations are metres;
 * display translations are mm. Presentation never changes the live matrix. */
export function HomogeneousMatrix({ matrix, name, symbols }: { matrix: Matrix4; name: string; symbols: string[] }) {
  return <figure className="afr2-homogeneous-matrix">
    <figcaption><span>Transformación homogenea</span><strong>{name}</strong></figcaption>
    <div className="afr2-homogeneous-matrix__equation">
      <span className="afr2-homogeneous-matrix__symbol" aria-hidden="true">T<sub>4×4</sub></span><span className="afr2-homogeneous-matrix__equals" aria-hidden="true">=</span>
      <div className="afr2-homogeneous-matrix__brackets"><table className="afr2-homogeneous-matrix__table" aria-label={`${name}; rotación sin unidad y posición en milímetros`}><tbody>
        {Array.from({ length: 4 }, (_, row) => <tr key={row}>{Array.from({ length: 4 }, (_, col) => <td key={col}
          data-region={row === 3 ? 'homogeneous' : col === 3 ? 'position' : 'rotation'} title={symbols[row * 4 + col]}>
          {cell(matrix.elements[col * 4 + row], row, col)}
        </td>)}</tr>)}
      </tbody></table></div>
    </div>
    <div className="afr2-homogeneous-matrix__legend"><span data-region="rotation"><i />R · rotación 3×3</span><span data-region="position"><i />p · posición en mm</span></div>
  </figure>;
}
